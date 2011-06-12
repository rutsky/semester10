#!/bin/bash

mkdir -p django

pushd django
hg clone https://bitbucket.org/wkornewald/django-nonrel
hg clone https://bitbucket.org/wkornewald/djangoappengine
hg clone https://bitbucket.org/wkornewald/djangotoolbox
hg clone https://bitbucket.org/twanschik/django-autoload
hg clone https://bitbucket.org/wkornewald/django-dbindexer
# Clone test project for convenience.
hg clone https://bitbucket.org/wkornewald/django-testapp
popd

mkdir -p server

pushd server
ln -s ../django/django-nonrel/django django
ln -s ../django/djangotoolbox/djangotoolbox djangotoolbox
ln -s ../django/django-autoload/autoload autoload
ln -s ../django/django-dbindexer/dbindexer dbindexer
ln -s ../django/djangoappengine djangoappengine
popd
