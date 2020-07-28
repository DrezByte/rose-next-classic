from django.dispatch import receiver
from allauth.account.signals import (
    email_confirmed,
    user_signed_up,
    password_set,
    password_changed,
    password_reset,
)
from pprint import pprint


@receiver(user_signed_up)
def on_user_signed_up(request, user, **kwargs):
    # TODO: Create a user in the game database
    # username: user.username
    # email: user.email
    # password: request.POST.password1
    # md5 hash the password
    pass


@receiver(email_confirmed)
def my_callback(request, email_address, **kwargs):
    # TODO: Update the user access_level based on email_address
    pass


@receiver([password_set, password_changed, password_reset])
def on_password_updated(request, user, **kwargs):
    # TODO: Update the user password based on username
    # username: user.username
    # password: request.POST.password1
    # md5 has the password
    pass
