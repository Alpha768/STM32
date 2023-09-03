# Keil-Autopiler v1.0

## 1 需求来源
部分对开发流程有限制的公司是将开发与编译（提交后的代码版本）分开的，要求编译过程尽可能少的人为参与，需在程序上实现代码的自动化编译。基于此需求开发了本脚本。
![演示界面](images/powershell_execute.png)

## 2 介绍
这是一个 keil 的自动化编译脚本，可被其他脚本或程序调用，接收参数并按参数编译 keil 工程，而不必打开 keil 软件，实现程序上的自动化。主要实现了以下功能：

1.  自动搜索脚本所在的目录下所有的 keil 工程
    - 搜索到多个 keil 工程时，若指定为空，则默认选择最后找到的 keil 工程
    - 指定 keil 工程时支持相对路径和绝对路径

2.  可选择 `Keil_v4` 或 `Keil_v5` 进行编译
    - 为空时默认使用 `Keil_v5` ，需指定 `Keil_v4` 输入 `4` 即可
    - 支持模糊输入，如： `K4` , `Keil4` , `Keilv4` , `Keil_4` 等
    - 支持指定不同 keil 软件的所在路径，如： `C:\Keil_v5\UV4\UV4.exe`
    - 当 keil 的安装路径和本案例中的 `D:\Keil_v5\UV4\UV4.exe` 路径不同时，除非修改代码后重新编译成新的 `.exe` ，否则必须指定 keil 软件的所在路径

3.  可选择 `project target`
    - 为空时默认选择 keil 工程最后选择的 `project target`
    - 支持非法字符识别并报错

4.  编译完成后自动将 `.bin` 和 `.hex` 文件拷贝至脚本的同级目录
    - 仅拷贝选定的 keil 工程编译出的 `.bin` 和 `.hex` 文件


## 3 有哪些参数
| 序号 | 描述 | 典型值 | 是否必须 |
| :---: | --- | --- | :---: |
| 1 | keil 版本 或 keil `UV4.exe` 的绝对路径 | 1. keil 路径与本脚本一致：`4` 或 `5` <br> 2. keil 路径与本脚本不一致： `C:\Keil_v5\UV4\UV4.exe` （示例）  | 否 |
| 2 | keil project target | release （示例） | 否 |
| 3 | 指定 keil 工程的相对路径或绝对路径 | 以 `.\` 或 `..\` 或 `C:\` 开头 | 否 |

> **说明：** 本脚本的所有参数可不按顺序输入，为空时表示选择默认值，但参数与参数之间需用**空格**隔开


## 4 我想直接用
0.  本脚本仅支持 `windows` 系统且要求已经安装了 keil 软件
1.  在发行版中下载可直接运行的 `.exe`
2.  将 `.exe` 脚本与需要编译的 keil 工程放在同一级目录（下称目录 A ）
3.  打开 `powershell` 或 `cmd` 并定位至目录 A 
    - 若使用 `powershell` ，可在目录 A 空白处按住 `shift` 键同时单击鼠标右键选择打开 `powershell` ，将自动定位到目录 A

4.  执行脚本
    - 案例一：
      - 只有一个 keil 工程或搜索到的最后一个工程就是要编译的
      - 只有一个 project target 
      - keil 软件的目录和脚本写好的一致
    ```
    .\Keil-Autopiler.exe
    ```

    - 案例二：
      - 指定 keil 版本
    ```
    .\Keil-Autopiler.exe 4
    ```
    或
    ```
    .\Keil-Autopiler.exe C:\Keil_v4\UV4\UV4.exe
    ```

    - 案例三：
      - 指定 keil 工程
    ```
    .\Keil-Autopiler.exe .\Code\Project\MDK\template.uvprojx
    ```
    或
    ```
    .\Keil-Autopiler.exe C:\Users\Administrator\Desktop\test\Code\Project\MDK\template.uvprojx
    ```

    - 案例四：
      - 指定 project target
    ```
    .\Keil-Autopiler.exe release
    ```

    - 案例五：
      - 指定 keil 版本
      - 指定 keil 工程
      - 指定 project target
    ```
    .\Keil-Autopiler.exe 4 release .\Code\Project\MDK\template.uvprojx
    ```
    或
    ```
    .\Keil-Autopiler.exe C:\Keil_v4\UV4\UV4.exe release C:\Users\Administrator\Desktop\test\Code\Project\MDK\template.uvprojx
    ```
5.  编译时间与编译器版本、编译选项和代码量有关，需耐心等待。（可打开同级目录的 `keilx_compile_log.txt` 查看编译过程日志）
6.  `powershell` 或 `cmd` 打印内容出现 `Copy above file(s) SUCCESS` 的字样表示执行成功，并将对应 keil 工程生成的 `.bin` 和 `.hex` 文件拷贝至脚本的同级目录中
    ![编译成功](images/keil_build_success.png)


## 6 我想自己编译
**本代码仅支持 windows 系统**
### 6.1 预备操作
0.  如果你已经安装了 gcc ，请忽略本步骤
1.  下载 gcc 编译器，为了考虑兼容性，这里提供一个 32 位的 mingw 下载链接： [i686-13.1.0-release-posix-dwarf-ucrt-rt_v11-rev1.7z](https://github.com/niXman/mingw-builds-binaries/releases/download/13.1.0-rt_v11-rev1/i686-13.1.0-release-posix-dwarf-ucrt-rt_v11-rev1.7z)
2.  解压后放在任意路径，此处以 `C:\mingw32` 为例
3.  配置好环境变量
    ![配置环境变量](images/path_config.png)

4.  打开 `powershell` 或 `cmd` 输入 `gcc -v` ，出现下图内容表示配置成功
    ![gcc](images/gcc.png)

### 6.2 编译
1.  打开 `powershell` 或 `cmd` 并定位至代码目录
    - 若使用 `powershell` ，可在代码目录空白处按住 `shift` 键同时单击鼠标右键选择打开 `powershell` ，将自动定位到代码目录

2.  执行以下 gcc 命令
    ```
    gcc .\Keil-Autopiler.c -o .\Keil-Autopiler.exe
    ```
3.  无任何提示信息，编译通过
    ![gcc编译通过](images/gcc_compile.png)


## 重要说明
> **本脚本依赖 keil 软件，定位为 keil 的自动化编译脚本。目的是替代手动编译，实现程序自动化，而非替代编译工具，并无附带任何编译器和链接器。因此，无论代码工程是基于 keil C51 亦或是 keil MDK 都可执行。**


## 参与贡献

1.  Fork 本仓库
2.  新建 Feat_xxx 分支
3.  提交代码
4.  新建 Pull Request

