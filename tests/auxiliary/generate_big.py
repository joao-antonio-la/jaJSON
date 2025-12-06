import json

N = 2000  # number of user objects to generate

data = {
    "meta": {
        "title": "Big JSON Stress Test",
        "version": 1,
        "generated": True
    },
    "data": []
}

for i in range(N):
    user = {
        "id": i,
        "active": (i % 2 == 0),
        "name": f"User{i}",
        "scores": list(range(i, i+10)),
        "profile": {
            "email": f"user{i}@example.com",
            "settings": {
                "theme": "dark" if i % 2 == 0 else "light",
                "language": "en" if i % 3 == 0 else "pt",
                "notifications": {
                    "email": bool(i % 2),
                    "sms": bool(i % 3),
                    "push": bool(i % 5)
                }
            }
        }
    }
    data["data"].append(user)

with open("tests/data/test_big.json", "w") as f:
    json.dump(data, f, indent=2)
