AIStreamer Ingestion Library - Docker & Kubernetes
===================================

We provide an example of [Docker image](../env/Dockerfile) for our customer to evaluate.

Please note that, use of third-party software is solely at our customers' discretion.
Google does not own the copyright of third-party software.

# Prerequiste

* Install [Docker](https://docs.docker.com/install)
* Install [Google Cloud SDK](https://cloud.google.com/sdk)
* Install [Kubernetes](https://kubernetes.io/docs/tasks/tools/install-kubectl/#download-as-part-of-the-google-cloud-sdk)

# Build docker image

Go into [ingestion directory](../) and run the following command line on your host machine:

```
$ export DOCKER_IMAGE=gcr.io/gcp-project-name/docker-image-name:version
$ docker build -t $DOCKER_IMAGE -f env/Dockerfile .
```

# Try docker image on your local host

Run following command line on your host machine:
```
$ docker run -it $DOCKER_IMAGE /bin/bash
```

It will return something like:
```
root@e504724e76fc:/#
```

To open another terminal connecting to Docker, run the following command line on host machine:
```
$ docker exec -it e504724e76fc /bin/bash
```

Now, you have both host terminals that are in the same docker container.

# Docker image environment setting

Some environment variables need to be set in the [Docker image](../env/Dockerfile).
```
#set up environment for Google Video Intelligence Streaming API
ENV SRC_DIR /googlesrc  #Source code directory
ENV BIN_DIR /google     #Binary directory
```

# Push docker image to GCP container registry

Run following command line on your host machine:
```
$ gcloud docker --verbosity debug -- push $DOCKER_IMAGE
```

You may need to properly set [access control](https://cloud.google.com/container-registry/docs/access-control).

# Deploy to GCP

Run following command line on your host machine:
```
$ export KUBE_ID=any_string_you_like
$ kubectl run -it $KUBE_ID --image=$DOCKER_IMAGE -- /bin/bash
```

It will return something like:
```
root@$KUBE_ID-215855480-c4sqp:/#
```

To open another terminal connecting to same Kubernetes container on GCP, run the following command line on host machine:
```
$ kubectl exec -it $KUBE_ID-215855480-c4sqp -- /bin/bash
```

Now, you have both host terminals that are in the same Kubernetes container on GCP.

