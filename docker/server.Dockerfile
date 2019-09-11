FROM python:alpine

# Install the required dependencies
COPY server /app

# Install librarium dependencies
WORKDIR /app
RUN pip install -r requirements.txt
