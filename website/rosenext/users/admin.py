from django.contrib import admin
from django.contrib.auth.admin import UserAdmin
from users.models import RoseNextUser

admin.site.register(RoseNextUser, UserAdmin)
