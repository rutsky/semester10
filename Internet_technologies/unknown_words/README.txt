Simple web site for learning unknown words from movies subtitles.
This is the course project for Internet technologies lessons in university.

Copyright (C) 2011  Vladimir Rutsky <altsysrq@gmail.com>

To setup local development environment:
 1. Setup Python 2.5 environment:
      $ virtualenv -p python2.5 --no-site-packages python
      $ ./python/bin/pip install PIL
      $ ./python/bin/pip install ssl
 2. Clone django-nonrel and setup links by running clone_django_nonrel.sh 
    script.
 3. Download latest Google App Engine Python SDK from 
    <http://code.google.com/appengine/downloads.html>, unzip it and set
    APP_ENGINE_SDK environment variable to unzip destination directory.
