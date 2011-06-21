from django.contrib.admin.models import LogEntry

FIELD_INDEXES = {
    LogEntry: {'indexed': ['object_id']},
}

# vim: ts=4 sw=4 et:
