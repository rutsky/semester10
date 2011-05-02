#  This file is part of risk calculation suite.
#
#  Copyright (C) 2011  Vladimir Rutsky <altsysrq@gmail.com>
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.

__author__  = "Vladimir Rutsky <altsysrq@gmail.com>"
__license__ = "GPL"

import sys
import signal
import traceback

import qtall as qt4

from main_window import MainWindow

def handle_int_signal(signum, frame):
    """Ask windows to close if Ctrl+C pressed."""
    print "test" # DEBUG
    qt4.qApp.closeAllWindows()

class IgnoreException(Exception):
    """A special exception class to be ignored by the exception handler."""

def excepthook(excepttype, exceptvalue, tracebackobj):
    """Show exception dialog if an exception occurs."""
    if not isinstance(exceptvalue, IgnoreException):
        # Next exception is ignored to clear out the stack frame of the
        # previous exception.

        msg_box = qt4.QMessageBox()
        msg_box.setText("".join(traceback.format_exception(
            excepttype, exceptvalue, tracebackobj)))
        msg_box.exec_()
        #raise IgnoreException

def main():
    # Load translator.
    translator = qt4.QTranslator()
    translator.load('i18n/ru_RU')

    # Create Qt application.
    app = qt4.QApplication(sys.argv)

    # Close application when all windows closed.
    app.lastWindowClosed.connect(app.quit)

    # Handle exceptions in Qt threads.
    sys.excepthook = excepthook

    # Register a signal handler to catch ctrl+C
    # TODO: Don't work
    signal.signal(signal.SIGINT, handle_int_signal)

    # Apply translator.
    app.installTranslator(translator)

    # Create main window.
    main_window = MainWindow()
    main_window.show()

    # Main loop.
    sys.exit(app.exec_())
    
if __name__ == "__main__":
    main()

# vim: set ts=4 sw=4 et:
