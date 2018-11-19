{
    "targets": [
    {
        "target_name": "image_threaded",
        "type": "shared_library",
        "sources": ["image_threaded.c"],
        "cflags": ["-fopenmp"],
        "libraries": ["-lgomp"]
    }
    ]
}
