Risk calculation suite.

Copyright (C) 2011  Vladimir Rutsky <altsysrq@gmail.com>


In Ubuntu 10.10 I used:
  * python v2.6.6 from default repository (python 2.6.6-2ubuntu2),
  * cx_freeze v4.2.3 from <http://cx-freeze.sourceforge.net/>,
  * PyQt v4.7.4 from default repository (python-qt4 4.7.4-0ubuntu1),
  * PyQwt v5.2.1 from default repository (python-qwt5-qt4 
    5.2.1~cvs20091107+dfsg-3).

In Windows XP I used:
  * python v2.6.6 from <http://python.org/>,
  * cx_freeze v4.2.2 from <http://cx-freeze.sourceforge.net/>,
  * PyQt v4.5.4 from <http://pyqwt.sourceforge.net/download.html>,
  * numpy v1.3.0 from <http://pyqwt.sourceforge.net/download.html>.


When dynamically loading UI file I observed following error:
  File "C:\python26\lib\site-packages\PyQt4\uic\Loader\qobjectcreator.py", line 24, in search
    self._module = __import__(self._moduleName, {}, {}, self._classes)
  TypeError: 'set' object does not support indexing

As workaround I found advice by Rosse Bertrand to 
"change the line 19 in the file
  C:\Python26\Lib\site-packages\PyQt4\uic\Loader\qobjectcreator.py:
    self._classes = set(classes)
  by
    self._classes = classes"
(<http://comments.gmane.org/gmane.comp.python.xy.devel/420>).


When running cx_freeze on Windows I observed following error:

  File "c:\python27\lib\site-packages\PyQt4\uic\port_v3\proxy_base.py", line 36
      class ProxyBase(metaclass=ProxyType):
      SyntaxError: invalid syntax

As workaround I renamed `PyQt4\uic\port_v3' directory to `PyQt4\uic\port_v3_'.
