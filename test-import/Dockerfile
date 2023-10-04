ARG BASE_IMAGE=node:18-alpine
FROM $BASE_IMAGE
ARG TARGETPLATFORM
ARG BUILDPLATFORM
RUN echo "Running on $BUILDPLATFORM, building for $TARGETPLATFORM";

WORKDIR /app
COPY package.json ./
COPY faiss-napi.tgz ./

RUN npm install ./faiss-napi.tgz --foreground-scripts
COPY . .

CMD [ "node", "index.js" ]