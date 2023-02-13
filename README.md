# webserv

This project is about writing your own HTTP server.
You will be able to test it with an actual browser.
HTTP is one of the most used protocols on the internet.
Knowing its arcane will be useful, even if you won’t be working on a website.

This project has been validated with a score of 100/100.

## Usage

⚠️ **ONLY WORKING ON LINUX** ⚠️

- Compile with `make`
- Go to `conf/simple.conf` and change the absolute path in nginx configuration to your working directory
- Execute `./webserv` to use the default configuration, or `./webserv conf/your_configuration.conf` to use your configuration
- Open your browser and go to `localhost:chosen_port`
