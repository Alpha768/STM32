/**
 * \file            Keil-Autopiler.c
 * \brief           main application
 */

/*
 * Copyright (c) 2023 Dino Haw
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is Keil-Autopiler - keil's automated compiler.
 *
 * Author:        Dino Haw <347341799@qq.com>
 * Version:       v1.0
 * Change Logs:
 * Date           Author       Notes
 * 2023-08-22     Dino         the first version
 */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <windows.h>


/* Defines -------------------------------------------------------------------*/
#define VERSION             "v1.0"

#define KEIL_UV_MARK        "UV4.exe"

#define MAX_DIR_HIERARCHY   32
#define MAX_PATH_QTY        10
#define MAX_PRJ_NAME_SIZE   64
#define MAX_COMPARE_CHAR    7

/* keil 工程路径存储链表 */
struct path_list
{
    char **items;
    size_t capacity;
    size_t size;
};


/* Private variables ---------------------------------------------------------*/
static char _current_dir[MAX_PATH];
static char _input_prj_path[MAX_PATH];
static char _input_prj_target[MAX_PRJ_NAME_SIZE];
static char _keil_uv_path[] = "D:\\Keil_v5\\UV4\\UV4.exe";
static char _log_file_name[] = "keil5_compile_log.txt";
static const char *_keil4_version[] = 
{
    "k4",
    "keil4",
    "keil_4",
    "keilv4",
    "keil_v4"
};
static const char *_keil5_version[] = 
{
    "k5",
    "keil5",
    "keil_5",
    "keilv5",
    "keil_v5"
};
static const char *_keil_prj_suffix[] = 
{
    ".uvprojx",
    ".uvproj"
};
static const char *_object_suffix[] = 
{
    ".hex",
    ".bin"
};
static const char *_build_file_suffix[] = 
{
    ".axf"
};
static const char *_not_prj_target = "<>:|/\\\"*?";


/* Private function prototypes -----------------------------------------------*/
bool is_keil                 (const char *str1, const char *str2[], size_t str2_qty);
bool is_keil_project         (const char *path);
bool is_keil_uv              (const char *str);
bool is_project_target       (const char *str, size_t str_len);
bool is_same_string          (const char *str1, const char *str2[], size_t str2_qty);
bool combine_path            (char *out_path, const char *absolute_path, const char *relative_path);
int  get_object_info         (const char *str, char *object_path, char *object_name);
void get_filename_from_path  (const char *path, char *filename);
int  parameter_process       (const char *param);
int  copy_files              (const char *dir, const char *paths[], size_t file_qty);
void search_files_with_suffix(const char *dir, 
                              const char *suffix[], 
                              size_t suffix_qty, 
                              struct path_list *list);
void search_files_with_name_and_suffix(const char *dir, 
                                       const char *name, 
                                       const char *suffix[],
                                       size_t suffix_qty,
                                       struct path_list *list);
struct path_list * path_list_init   (size_t capacity);
void               path_list_add    (struct path_list *list, char *item);
void               path_list_free   (struct path_list *list);


/* Exported functions ---------------------------------------------------------*/
/**
 * @brief  主函数入口
 * @note   
 * @param  argc: 参数数量
 * @param  argv[0]: 程序路径
 * @param  argv[1]: keil 版本 (4 | 5) 或 keil UV4.exe 的绝对路径
 * @param  argv[2]: keil project target
 * @param  argv[3]: 指定的工程路径，支持相对和绝对路径
 * @retval 0: 成功 | -1: 失败
 */
int main(int argc, char *argv[])
{
    /* 获取本程序所在目录 */
    GetModuleFileName(NULL, _current_dir, MAX_PATH);

    char *last_slash = strrchr(_current_dir, '\\');
    if (last_slash != NULL) {
        *last_slash = '\0';
    }

    /* 1. 参数合法性检查和转换 */
    if (argc > 1)
    {
        if (parameter_process(argv[1]) != 0) 
        {
            printf("[ERROR] NOT support %s\n", argv[1]);
            printf("[Tips] Parameter 1st standard input: keil version (4 or 5)\n\n");
            return -1;
        }
    }

    if (argc > 2)
    {
        if (parameter_process(argv[2]) != 0) 
        {
            printf("[ERROR] NOT support %s\n", argv[2]);
            printf("[Tips] Parameter 2nd standard input: keil project target.\n");
            printf("[Tips] If you don't know what 'project target' is or don't need to specify 'project target', just ignore parameter 2nd.\n\n");
            return -1;
        }
    }

    /* 指定工程路径 */
    if (argc > 3)
    {
        if (parameter_process(argv[3]) != 0)
        {
            printf("[ERROR] NOT support %s\n", argv[3]);
            printf("[Tips] Parameter 3nd standard input: keil project relative path with the suffix '.uvprojx' or '.uvproj'\n");
            printf("[Tips] If you don't need to specify keil project, just ignore parameter 3nd.\n");
            printf("[Tips] Example: .\\Keil-Autopiler.exe 5 Debug .\\Code\\Project\\MDK`(AC6`)\\project.uvprojx\n\n");
            return -1;
        }
    }

    /* 2. 拼接日志路径 */
    char log_file_path[MAX_PATH];
    snprintf(log_file_path, sizeof(log_file_path), "%s\\%s", _current_dir, _log_file_name);

    printf("\n------------------------------------- Keil-Autopiler %s --------------------------------------\n", VERSION);
    printf("Keil path: %s\n", _keil_uv_path);
    printf("Build log path: %s\n", log_file_path);

    /* 初始化存放工程路径的数组 */
    struct path_list *prj_list = path_list_init(MAX_PATH_QTY);

    /* 3. 遍历目录，查找 keil 工程 */
    search_files_with_suffix( _current_dir, 
                              _keil_prj_suffix, 
                              sizeof(_keil_prj_suffix) / sizeof(_keil_prj_suffix[0]), 
                              prj_list);

    /* 4. 打印找到的所有 keil 工程路径 */
    if (prj_list->size == 0 && _input_prj_path[0] == '\0')
    {
        printf("\n[ERROR] NO keil project found\n\n");
        return -1;
    }
    else
    {
        printf("\nSearch keil project: %d item(s)\n", prj_list->size);
        for (size_t i = 0; i < prj_list->size; i++) 
        {
            printf("\t%s\n", prj_list->items[i]);
        }
        printf("\n");
    }

    /* 用户指定了 keil 工程 */
    char *prj_path = prj_list->items[prj_list->size-1];
    if (_input_prj_path[0] != '\0')
    {
        prj_path = _input_prj_path;
        printf("[Hint] You specify the keil project!\n");
    }

    /* 5. 打印将被编译的工程 */
    char prj_name[MAX_PRJ_NAME_SIZE] = {0};
    get_filename_from_path(prj_path, prj_name);
    printf("Build project name  : %s\n", prj_name);
    if (_input_prj_target[0] != '\0') 
    {
        printf("Build project target: %s\n", _input_prj_target);
    }
    printf("Build project path  : %s\n", prj_path);

    /* 6. 打印编译时间 */
    time_t raw_time;
    struct tm *time_info;
    time(&raw_time);
    time_info = localtime(&raw_time);
    printf("Build date and time : %.4d/%.2d/%.2d %.2d:%.2d:%.2d\n", 
                                  time_info->tm_year + 1900, time_info->tm_mon + 1, time_info->tm_mday,
                                  time_info->tm_hour, time_info->tm_min, time_info->tm_sec);

    /* 7. 拼接编译命令 */
    char command[256];
    if (_input_prj_target[0] == '\0')
    {
        snprintf(command, sizeof(command), "%s -j0 -r %s -o %s", 
                                            _keil_uv_path, prj_path, log_file_path);
    }
    else
    {
        snprintf(command, sizeof(command), "%s -j0 -r %s -t %s -o %s", 
                                            _keil_uv_path, prj_path, _input_prj_target, log_file_path);
    }
    printf("\n%s\n", command);

    /* 8. 清空日志内容 */
    FILE *fp = fopen(log_file_path, "w+");
    if (fp != NULL) {
        fclose(fp);
    }

    printf("\nGet more information about command line:\n");
    printf("https://developer.arm.com/documentation/101407/0538/Command-Line?lang=en\n\n");
    printf("Please wait for keil_v%c build complete .........\n\n", _keil_uv_path[9]);
    printf("------------------------------------------------------------------------------------------------\n\n");

    /* 9. 执行编译命令 */
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));
    si.cb = sizeof(si);

    int result = CreateProcess(NULL, command, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
    if (result == 0) 
    {
        printf("[ERROR] Keil_v%c fails to build, please check the keil project or keil path\n\n", _keil_uv_path[9]);
        goto __exit;
    }
    
    /* 等待进程结束 */
    WaitForSingleObject(pi.hProcess, INFINITE);
    
    /* 获取命令行返回的值 */
    DWORD exit_code = -1;
    GetExitCodeProcess(pi.hProcess, &exit_code);

    /* 关闭句柄 */
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    /* 10. 打印编译日志并获取日志中 object 文件的相对路径 */
    int  info_result = 1;
    char object_name[MAX_PRJ_NAME_SIZE] = {0};
    char object_relative_path[MAX_PATH] = {0};
    FILE *file = fopen(log_file_path, "r");
    if (file != NULL) 
    {
        char line[256];
        while (fgets(line, sizeof(line), file))     /* 逐行读取 */
        { 
            /* 如果是 object 文件的相对路径一行，则处理并保存 */
            if (line[0] == '"' 
            &&  line[1] == '.' 
            &&  strstr(line, "Error(s)") != NULL
            &&  strstr(line, "Warning(s)") != NULL)
            {
                info_result = get_object_info(line, object_relative_path, object_name);
            }
            printf("%s", line);
        }
        fclose(file);
    }

    /* 获取 object 文件的相对路径出错 */
    if (info_result == 1 
    &&  (exit_code == 0 || exit_code == 1))
    {
        printf("[ERROR] can't get the object relative path\n\n");
        printf("exit_code: %d\n", exit_code);
        goto __exit;
    }
    
    if (info_result == -1) 
    {
        printf("[ERROR] object path(s) in this format are not supported\n\n");
        goto __exit;
    }
    else if (info_result == -2) 
    {
        printf("[ERROR] can't get the object name\n\n");
        goto __exit;
    }

    /* 11. 打印编译结果 */
    char log[256] = {0};
    printf("\n");
    switch (exit_code)
    {
        case 0:
            printf("------------------------------------------- SUCCESS -------------------------------------------\n\n");
            break;
        case 1:
            printf("------------------------------------ SUCCESS with warnings ------------------------------------\n\n");
            break;
        case 2:
            strncpy_s(log, sizeof(log), "[keil info] Errors (2)\n\n", 
                                 strlen("[keil info] Errors (2)\n\n"));
            break;
        case 3:
            strncpy_s(log, sizeof(log), "[keil info] Fatal Errors (3)\n\n", 
                                 strlen("[keil info] Fatal Errors (3)\n\n"));
            break;
        case 11:
            strncpy_s(log, sizeof(log), "[keil info] ERROR: Cannot open project file for writing (11)\n\n", 
                                 strlen("[keil info] ERROR: Cannot open project file for writing (11)\n\n"));
            break;
        case 12:
            strncpy_s(log, sizeof(log), "[keil info] ERROR: Device with given name in not found in database (12)\n\n", 
                                 strlen("[keil info] ERROR: Device with given name in not found in database (12)\n\n"));
            break;
        case 13:
            strncpy_s(log, sizeof(log), "[keil info] ERROR: writing project file (13)\n\n", 
                                 strlen("[keil info] ERROR: writing project file (13)\n\n"));
            break;
        case 15:
            strncpy_s(log, sizeof(log), "[keil info] ERROR: reading import XML file (15)\n\n", 
                                 strlen("[keil info] ERROR: reading import XML file (15)\n\n"));
            break;
        case 20:
            strncpy_s(log, sizeof(log), "[keil info] ERROR: converting project (20)\n\n", 
                                 strlen("[keil info] ERROR: converting project (20)\n\n"));
            break;
        default: 
            strncpy_s(log, sizeof(log), "[keil info] ERROR: Unknown Errors\n\n", 
                                 strlen("[keil info] ERROR: Unknown Errors\n\n"));
            break;
    }
    printf(log);
    
    /* 12. 出现错误则将错误信息追加到日志文件中 */
    if (exit_code > 1)
    {
        FILE *fp = fopen(log_file_path, "a+");

        if (fp == NULL) {
            fp = fopen(log_file_path, "w+");
        } else {
            fseek(fp, 0, SEEK_END);
        }
        fputs(log, fp);

        fclose(fp);
    }
    /* 12. 搬运生成的 hex 和 bin 文件至本程序的同级目录 */
    else
    {
        char object_dir[MAX_PATH] = {0};

        if (combine_path(object_dir, prj_path, object_relative_path) == false) {
            goto __exit;
        }

        struct path_list *object_list = path_list_init(MAX_PATH_QTY);

        search_files_with_name_and_suffix( object_dir, 
                                           object_name,
                                           _object_suffix,
                                           sizeof(_object_suffix) / sizeof(_object_suffix[0]),
                                           object_list);

        /* 打印找到的所有 object 文件的路径 */
        if (object_list->size == 0)
        {
            printf("[ERROR] NO follow files found in %s\n", object_dir);
            for (size_t i = 0; i < sizeof(_object_suffix) / sizeof(_object_suffix[0]); i++) 
            {
                printf("\t%s%s\n", object_name, _object_suffix[i]);
            }
            printf("\n");
            path_list_free(object_list);
            goto __exit;
        }
        else
        {
            printf("Search object file(s): %d item(s)\n", object_list->size);
            for (size_t i = 0; i < object_list->size; i++) 
            {
                printf("\t%s\n", object_list->items[i]);
            }
            printf("\n");
        }

        /* 搬运文件 */
        int move_result = copy_files(_current_dir, (const char **)object_list->items, object_list->size);
        if (move_result == 0) {
            printf("Copy above file(s) SUCCESS ==> %s\n\n", _current_dir);
        } else {
            printf("\n");
        }

        path_list_free(object_list);
    }

__exit:
    path_list_free(prj_list);

    return 0;
}


/**
 * @brief  参数 1 和 参数 2 的处理
 * @note   
 * @param  param: 参数 1 或 参数 2 ，详见 main 函数
 * @retval 0: 成功 | -1: 失败
 */
int parameter_process(const char *param)
{
    /* 指定了 keil UV4.exe 的路径 */
    if (is_keil_uv(param))
    {
        strncpy_s(_keil_uv_path, sizeof(_keil_uv_path), param, strnlen_s(param, MAX_PATH));
        strncpy_s(_log_file_name, sizeof(_log_file_name), "keil_compile_log.txt", strlen("keil_compile_log.txt"));
    }
    /* 指定了 keil4 版本 */
    else if (strspn(param, "0123456789") == strnlen_s(param, MAX_PRJ_NAME_SIZE)
    &&       param[1] == '\0')
    {
        if (param[0] == '4')
        {
            _keil_uv_path[9]  = '4';
            _log_file_name[4] = '4';
        }
        else if (param[0] != '5') {
            return -1;
        }
    }
    else if (is_keil(param, _keil4_version, sizeof(_keil4_version) / sizeof(_keil4_version[0])))
    {
        _keil_uv_path[9]  = '4';
        _log_file_name[4] = '4';
    } 
    /* 指定了要编译的 keil 工程 */
    else if (is_keil_project(param)) 
    {
        strncpy_s(_input_prj_path, sizeof(_input_prj_path), _current_dir, strnlen_s(_current_dir, sizeof(_current_dir)));

        /* 路径字符串拼接 */
        if (param[0] == '.' && param[1] == '\\')
        {
            strncat_s(_input_prj_path, sizeof(_input_prj_path), &param[1], strnlen_s(&param[1], MAX_PATH));
        }
        else if (param[0] == '\\')
        {
            strncat_s(_input_prj_path, sizeof(_input_prj_path), param, strnlen_s(param, MAX_PATH));
        }
        else if (param[1] == ':')
        {
            strncpy_s(_input_prj_path, sizeof(_input_prj_path), param, strnlen_s(param, MAX_PATH));
        }
        else
        {
            /* 同级目录时，工程相对路径可不以 .\ 或 \ 开头 */
            char slash[] = "\\";
            strncat_s(_input_prj_path, sizeof(_input_prj_path), slash, sizeof(slash));
            strncat_s(_input_prj_path, sizeof(_input_prj_path), param, strnlen_s(param, MAX_PATH));
        }
    }
    /* 指定了 keil5 版本 */
    else if (is_keil(param, _keil5_version, sizeof(_keil5_version) / sizeof(_keil5_version[0])))
    {
        return 0;
    }
    /* project target */
    else if (is_project_target(param, strnlen_s(param, MAX_PRJ_NAME_SIZE)))
    {
        strncpy_s(_input_prj_target, sizeof(_input_prj_target), param, strnlen_s(param, MAX_PRJ_NAME_SIZE));
    }
    else
    {
        return -1;
    }

    return 0;
}


/**
 * @brief  拼接路径
 * @note   
 * @param  out_path: 拼接后输出的路径
 * @param  absolute_path: 输入的绝对路径
 * @param  relative_path: 输入的相对路径
 * @retval true: 成功 | false: 失败
 */
bool combine_path(char *out_path, const char *absolute_path, const char *relative_path)
{
    if (relative_path[0] != '.')
    {
        printf("[ERROR] relative path can only start with .\\ or ..\\\n\n");
        return false;
    }

    /* 1. 将绝对路径 absolute_path 的文件名和扩展名去除 */
    strncpy_s(out_path, MAX_PATH, absolute_path, strnlen_s(absolute_path, MAX_PATH));

    char *last_slash = strrchr(out_path, '\\');
    if (last_slash != NULL)
    {
        /* 说明不是是盘符根目录 */
        if (*(last_slash - 1) != ':') {
            *last_slash = '\0';
        }
    }
    else
    {
        printf("[ERROR] %s not a absolute path\n\n", absolute_path);
        return false;
    }

    /* 2. 读取绝对路径的目录层级 并 记录每一层级的相对偏移值 */
    size_t hierarchy_count = 0;
    size_t dir_hierarchy[MAX_DIR_HIERARCHY];

    /* 逐字符遍历路径 */
    for (size_t i = 0; i < strnlen_s(out_path, MAX_PATH); i++)
    {
        if (out_path[i] == '\\')
        {
            dir_hierarchy[hierarchy_count++] = i;

            if (hierarchy_count >= MAX_DIR_HIERARCHY) {
                break;
            }
        }
    }

    /* 3. 读取相对路径 relative_path 的向上层级数 */
    size_t dir_up_count = 0;
    size_t valid_path_offset = 0;

    for (size_t i = 0; i < strnlen_s(relative_path, MAX_PATH); )
    {
        if (relative_path[i]   == '.' 
        &&  relative_path[i+1] == '.' 
        &&  relative_path[i+2] == '\\')
        {
            i += 3;
            dir_up_count++;
            valid_path_offset += 3;
        }
        else if (relative_path[i]   == '.' 
        &&       relative_path[i+1] == '\\') 
        {
            valid_path_offset = 2;
            break;
        }
        else {
            break;
        }
    }

    /* 4. 根据 3 获得的级数，缩减绝对路径的目录层级 */
    if (dir_up_count > 0)
    {
        if ((hierarchy_count - dir_up_count) > 0)
        {
            hierarchy_count -= (dir_up_count - 1);
            size_t offset = dir_hierarchy[hierarchy_count - 1];
            out_path[offset] = '\0';
        }
        else
        {
            printf("[ERROR] relative paths go up more levels than absolute paths\n\n");
            return false;
        }
    }

    /* 5. 根据 2 记录的偏移值和 3 获得的级数，将 absolute_path 和 relative_path 拼接 */
    strncat_s(out_path, MAX_PATH, "\\", 1);
    strncat_s(out_path, MAX_PATH, &relative_path[valid_path_offset], strnlen_s(&relative_path[valid_path_offset], MAX_PATH));

    return true;
}


/**
 * @brief  从路径中获取文件名
 * @note   
 * @param  path: 路径
 * @param  filename: 文件名
 * @retval None
 */
void get_filename_from_path(const char *path, char *filename)
{
    char *p = strrchr(path, '\\');

    if (p != NULL)
    {
        p++;    /* 跳过 \ */
        strncpy_s(filename, MAX_PRJ_NAME_SIZE, p, strnlen_s(p, MAX_PATH));
    } 
    else 
    {
        /* 没有 \ 默认是文件名 */
        strncpy_s(filename, MAX_PRJ_NAME_SIZE, path, strnlen_s(path, MAX_PATH));
    }
}


/**
 * @brief  获取 object 的相对路径和文件名
 * @note   
 * @param  str: 日志信息
 * @param  object_path: object 所在的相对路径
 * @param  object_name: object 的文件名
 * @retval 0: 成功 | -1 -2: 失败
 */
int get_object_info(const char *str, char *object_path, char *object_name)
{
    /* ".\list\MPC_VerB_MCU_V1_0_2_1-20230808" - 0 Error(s), 0 Warning(s). */
    size_t i = 0;
    bool second_quote = false;

    /* 截取相对路径 */
    while (*str != '\0')
    {
        if (*str == '"')
        {
            if (second_quote) {
                break;
            }
            str++;
            second_quote = true;
        }
        if (second_quote) {
            object_path[i++] = *str;
        }
        str++;
    }

    if (second_quote == false) {
        return -1;
    }

    /* 截取文件名 */
    char *last_slash = strrchr(object_path, '\\');
    if (last_slash != NULL)
    {
        size_t len = strnlen_s(object_path, MAX_PATH) - (last_slash - object_path);
        strncpy_s(object_name, MAX_PRJ_NAME_SIZE, last_slash + 1, len);

        /* 仅保留目录层级，去掉文件名 */
        *last_slash = '\0';
    }
    else {
        return -2;
    }

    /* 截取文件名 */
    for (size_t i = 0; i < sizeof(_build_file_suffix) / sizeof(_build_file_suffix[0]); i++)
    {
        if (strstr(object_name, _build_file_suffix[i]) != NULL) 
        {
            char *last_point = strrchr(object_name, '.');
            *last_point = '\0';
        }
    }

    return 0;
}


/**
 * @brief  某路径是否为 keil 工程
 * @note   
 * @param  path: 路径
 * @retval true: 是 | false: 否
 */
bool is_keil_project(const char *path)
{
    for (size_t i = 0; i < sizeof(_keil_prj_suffix) / sizeof(_keil_prj_suffix[0]); i++)
    {
        if (strstr(path, _keil_prj_suffix[i]) != NULL) {
            return true;
        }
    }

    return false;
}


/**
 * @brief  字符串比对
 * @note   
 * @param  str1: 字符串 1
 * @param  str2[]: 字符串组 2
 * @param  str2_qty: 字符串组 2 的数量
 * @retval true: 一致 | false: 不一致
 */
bool is_same_string(const char *str1, const char *str2[], size_t str2_qty)
{
    for (size_t i = 0; i < str2_qty; i++)
    {
        if (strcmp(str1, str2[i]) == 0) {
            return true;
        }
    }

    return false;
}


/**
 * @brief  是否为 keil
 * @note   
 * @param  str1: 字符串 1
 * @param  str2[]: 字符串组 2
 * @param  str2_qty: 字符串组 2 数量
 * @retval true: 是 | false: 否
 */
bool is_keil(const char *str1, const char *str2[], size_t str2_qty)
{
    size_t c;

    for (size_t i = 0; i < str2_qty; i++)
    {
        size_t count = strnlen_s(str2[i], MAX_COMPARE_CHAR);
        for (c = 0; c < count; c++) 
        {
            if (tolower(str1[c]) != str2[i][c]) {
                break;
            }
        }
        if (c == count) {
            return true;
        }
    }

    return false;
}


/**
 * @brief  是否为 keil UV 程序
 * @note   
 * @param  str: 字符串
 * @retval true: 是 | false: 否
 */
bool is_keil_uv(const char *str)
{
    if (strstr(str, KEIL_UV_MARK) != NULL) {
        return true;
    }

    return false;
}


/**
 * @brief  是否为 project target
 * @note   
 * @param  str: 字符串
 * @param  str_len: 字符串长度
 * @retval true: 是 | false: 否
 */
bool is_project_target(const char *str, size_t str_len)
{
    for (size_t i = 0; i < str_len; i++)
    {
        for (size_t k = 0; k < strlen(_not_prj_target); k++)
        {
            if (str[i] == _not_prj_target[k]) {
                return false;
            }
        }
    }

    return true;
}


/**
 * @brief  初始化动态列表
 * @note   
 * @param  capacity: 列表容量
 * @retval struct path_list *
 */
struct path_list *path_list_init(size_t capacity)
{
    struct path_list *list = malloc(sizeof(struct path_list));

    list->items    = malloc(capacity * sizeof(char *));
    list->capacity = capacity;
    list->size     = 0;

    return list;
}


/**
 * @brief  向动态列表添加元素
 * @note   
 * @param  list: 列表对象
 * @param  item: 要新增的项
 * @retval None
 */
void path_list_add(struct path_list *list, char *item)
{
    /* 如果数组已满，将其最大容量翻倍 */
    if (list->size == list->capacity)
    {
        list->capacity *= 2;
        list->items = realloc(list->items, list->capacity * sizeof(char *));
    }
    list->items[list->size++] = item;
}


/**
 * @brief  释放动态列表
 * @note   
 * @param  list: 列表对象
 * @retval None
 */
void path_list_free(struct path_list *list)
{
    for (size_t i = 0; i < list->size; i++) {
        free(list->items[i]);
    }
    free(list->items);
    free(list);
}


/**
 * @brief  按文件名和扩展名搜索文件
 * @note   本函数不会递归搜索
 * @param  dir: 要搜索的目录
 * @param  name: 文件名
 * @param  suffix[]: 扩展名，支持多个
 * @param  suffix_qty: 扩展名数量
 * @param  list: 保存搜索到的文件路径
 * @retval None
 */
void search_files_with_name_and_suffix( const char *dir, 
                                        const char *name, 
                                        const char *suffix[],
                                        size_t suffix_qty,
                                        struct path_list *list)
{
    HANDLE h_find;
    WIN32_FIND_DATA find_data;
    char file_name[MAX_PRJ_NAME_SIZE] = {0};

    /* 加上 '*' 以搜索所有文件和文件夹 */
    char path[MAX_PATH];
    snprintf(path, sizeof(path), "%s\\*", dir);

    /* 开始搜索 */
    h_find = FindFirstFile(path, &find_data);
    if (h_find == INVALID_HANDLE_VALUE) {
        return;
    }

    do {
        char *str = strrchr(find_data.cFileName, '.');
        if (str != NULL)
        {
            if (is_same_string(str, suffix, suffix_qty) == false) {
                continue;
            }

            strncpy_s(file_name, sizeof(file_name), find_data.cFileName, strnlen_s(find_data.cFileName, sizeof(find_data.cFileName)));

            char *last_point = strrchr(file_name, '.');
            if (last_point != NULL) {
                *last_point = '\0';
            }

            const char **pp_name = &name;
            if (is_same_string(file_name, pp_name, 1))
            {
                char *file_path = malloc(MAX_PATH * sizeof(char));
                snprintf(file_path, MAX_PATH * sizeof(char), "%s\\%s", dir, find_data.cFileName);
                path_list_add(list, file_path);
            }
        }
    } while (FindNextFile(h_find, &find_data) != 0);

    FindClose(h_find);
}


/**
 * @brief  按扩展名搜索文件
 * @note   本函数会递归搜索
 * @param  dir: 要搜索的目录
 * @param  suffix[]: 扩展名，支持多个
 * @param  suffix_qty: 扩展名数量
 * @param  list: 保存搜索到的文件路径
 * @retval None
 */
void search_files_with_suffix(const char *dir, const char *suffix[], size_t suffix_qty, struct path_list *list)
{
    HANDLE h_find;
    WIN32_FIND_DATA find_data;

    /* 加上 '*' 以搜索所有文件和文件夹 */
    char path[MAX_PATH];
    snprintf(path, sizeof(path), "%s\\*", dir);

    /* 开始搜索 */
    h_find = FindFirstFile(path, &find_data);
    if (h_find == INVALID_HANDLE_VALUE) {
        return;
    }

    do {
        /* 如果找到的是目录，且不是 "." 或 ".." ，则递归搜索该目录 */
        if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            if (strcmp(find_data.cFileName, ".")  != 0 
            &&  strcmp(find_data.cFileName, "..") != 0)
            {
                char sub_dir[MAX_PATH] = {0};
                snprintf(sub_dir, sizeof(sub_dir), "%s\\%s", dir, find_data.cFileName);
                search_files_with_suffix(sub_dir, suffix, suffix_qty, list);
            }
        }
        /* 如果找到的是文件，判断其后缀是否为 keil 工程 */
        else
        {
            char *str = strrchr(find_data.cFileName, '.');
            if (str && is_same_string(str, suffix, suffix_qty))
            {
                char *file_path = malloc(MAX_PATH * sizeof(char));
                snprintf(file_path, MAX_PATH * sizeof(char), "%s\\%s", dir, find_data.cFileName);
                path_list_add(list, file_path);
            }
        }
    } while (FindNextFile(h_find, &find_data) != 0);

    FindClose(h_find);
}


/**
 * @brief  复制文件
 * @note   
 * @param  dir: 要复制到的目录
 * @param  paths[]: 被复制的路径，支持多个
 * @param  file_qty: 文件数量
 * @retval 0: 成功 | -1: 失败
 */
int copy_files(const char *dir, const char *paths[], size_t file_qty)
{
    WINBOOL result;
    char new_path[MAX_PATH];

    for (size_t i = 0; i < file_qty; i++)
    {
        char *file_name = strrchr(paths[i], '\\');
        if (file_name != NULL) 
        {
            strncpy_s(new_path, sizeof(new_path), dir, strnlen_s(dir, MAX_PATH));
            strncat_s(new_path, sizeof(new_path), file_name, strnlen_s(file_name, MAX_PATH));
        } 
        else {
            continue;
        }

        result = CopyFile(paths[i], new_path, FALSE);
        if (result == 0) {
            printf("[ERROR] copy %s to %s\n", paths[i], dir);
        }
    }

    if (result == 0) {
        return -1;
    }

    return 0;
}
