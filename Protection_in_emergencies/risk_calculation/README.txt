Risk calculation suite.

Copyright (C) 2011  Vladimir Rutsky <altsysrq@gmail.com>


In Ubuntu 10.10 I used:
  * python v2.6.6 from default repository (python 2.6.6-2ubuntu2),
  * cx_freeze v4.2.3 from <http://cx-freeze.sourceforge.net/>,
  * PyQt v4.7.4 from default repository (python-qt4 4.7.4-0ubuntu1).

In Windows XP I used:
  * python v2.7.1 from <http://python.org/>,
  * cx_freeze v4.2.3 from <http://cx-freeze.sourceforge.net/>,
  * PyQt v4.8.3 from <http://www.riverbankcomputing.com/software/pyqt>.

When running cx_freeze on Windows I observed following error:

  File "c:\python27\lib\site-packages\PyQt4\uic\port_v3\proxy_base.py", line 36
    class ProxyBase(metaclass=ProxyType):
  SyntaxError: invalid syntax

As workaround I renamed `PyQt4\uic\port_v3' directory to `PyQt4\uic\port_v3_'.
