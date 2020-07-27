import json

from django.contrib.auth.decorators import login_required
from django.http.response import HttpResponse


@login_required()
def oauth_profile(request, *args, **kwargs):
    user_info = {
        "id": request.user.id,
        "name": f"{request.user.username}",
        "email": request.user.email,
    }
    return HttpResponse(json.dumps(user_info), status=200)

