from django.urls import include, path
from users.views import oauth_profile

urlpatterns = [
    path("oauth/profile/", oauth_profile),
]
