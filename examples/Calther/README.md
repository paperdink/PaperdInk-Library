# Calther
This example displays a calender along with weather and todo list.
Todo list can be synced from [Taiga](https://www.taiga.io) or [Todoist](https://todoist.com) and weather from [Openweathermap](https://www.openweathermap.org)

<img src="Calther.png" width="500" alt="Calther image">

## Usage

### Getting Openweathermap API key
From their [website](https://openweathermap.org/appid),

The API key is all you need to call any of our weather APIs.
Once you [sign up](https://openweathermap.org/home/sign_up) using your email,
the API key (APPID) will be sent to you in a confirmation email.
Your API keys can always be found on your [account page](https://home.openweathermap.org/api_keys),
where you can also generate additional API keys if needed. 
Check our [documentation page](https://openweathermap.org/api)
to find all technical information for each product. 
Documentation is an essential guide with actual examples and 
comprehensive description of API calls, responses and parameters.

### Todo list from Taiga
First create an account on [Taiga](https://www.taiga.io) and create a new project.
Add your login details and project name to the config file.

### Todo list from Todoist
First create an account on [Todoist](https://www.todoist.com).
Go to `Settings > Integrations` and generate an API token.
Copy this token to the config file.

### Update `config.h`

Update the `config.h` file with configuration details
