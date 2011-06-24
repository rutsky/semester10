Simple web site for learning unknown words from serials subtitles.
This is the course project for Internet technologies lessons in university.

Copyright (C) 2011  Vladimir Rutsky <altsysrq@gmail.com>

To setup local development environment:
 1. Setup Python 2.5 environment:
      $ virtualenv -p python2.5 --no-site-packages python
      $ ./python/bin/pip install PIL
      $ ./python/bin/pip install ssl
    Note that my installation of SSL wasn't successfully finished.
 2. Clone django-nonrel and setup links by running clone_django_nonrel.sh 
    script.
 3. Download latest Google App Engine Python SDK from 
    <http://code.google.com/appengine/downloads.html>, unzip it and set
    APP_ENGINE_SDK environment variable to unzip destination directory.

To deploy:
 1. Change in app.yaml application name (`application: ...') to you 
    application name registered at <https://appengine.google.com/start>.
 2. Create custom server/secret.py (see server/secret.py.example for example).
 3. $ ./manage.py deploy
