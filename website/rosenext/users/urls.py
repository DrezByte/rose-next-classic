from django.contrib.auth.decorators import login_required
from django.urls import include, path

from users.views import oauth_profile, ProfileView

app_name = "users"

urlpatterns = [
    path("oauth/profile/", oauth_profile, name="oauth_profile"),
    path("profile/", login_required(ProfileView.as_view()), name="profile"),
]
