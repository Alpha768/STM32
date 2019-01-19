#coding:utf8
from typing import Union


class ChinaGetter:
    """A simple localizer a la gettext"""
    def __init__(self):
        self.trans = dict(dog = u"dog",cat = u"cat")

    def get(self,msgid):
        """We'll punt if we don't have a translation"""
        try:
            return self.trans[msgid]
        except KeyError:
            return str(msgid)



class EnglishGetter:
    """Simply echoes the msg ids"""
    def __get__(self, msgid):
        return str(msgid)

def get_localizer(language="English"):
    """The factory method"""
    language = dict(English=EnglishGetter,China=ChinaGetter)
    return language[language]()


# Create our localizers


#e: Union[ChinaGetter, EnglishGetter]=get_localizer("English")
#g=get_localizer("China")

#Localize some text
#for msgid in "dog parrot cat bear".split():
 #   print(e.get(msgid),g.get(msgid))










