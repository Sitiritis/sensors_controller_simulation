FROM nginx:alpine

# Generate certificate
RUN apk update && apk add --no-cache\
    openssl

RUN mkdir /certs

RUN openssl req -x509 -nodes -days 385000 -newkey rsa:2048 -keyout /certs/private.key -out /certs/selfsigned.crt -subj "/C=RU/ST=Tatarstan/L=Innopolis/O=Me/OU=Me"

RUN openssl dhparam -out /certs/dhparam.pem 2048
