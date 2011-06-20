from django.conf.urls.defaults import *

handler500 = 'djangotoolbox.errorviews.server_error'

urlpatterns = patterns('',
    ('^_ah/warmup$', 'djangoappengine.views.warmup'),
#    ('^$', 'django.views.generic.simple.direct_to_template',
#     {'template': 'home.html'}),
    (r'^$', include('learnsubtitles.urls'))
)

# vim: ts=4 sw=4 et:
