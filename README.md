# IOT Project 

Welcome to the IOT project! This repository contains everything you need to get the backend and website server running for the project. Follow the steps below to get started.

| Welcome to the IOT project! This repository contains everything you need to get the backend and website server running for the project. Follow the steps below to get started. | |
| ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------- |

## Prerequisites

Before you begin, ensure you have the following installed:

1. **Node.js** (Latest version) - [Download here](https://nodejs.org)
2. **Docker** - [Download here](https://www.docker.com/products/docker-desktop)

---

## Setup Instructions

### 1. Clone the Repository

Pull the latest version of this repository to your local machine:

```bash
git clone https://github.com/HF-SDE/HF3-Team1-io.git
cd HF3-Team1-io
```

### 2. Create and fill out the .env files

Please create and fill out all the .env files in the project. Both for the frontend and backend. Their is an example file for every file that needs to be created just replace example with dev in the .env file name.

### 3. Install the packages

Install the latest version of the required packages:

```bash
npm install
```

### 4. Build the docker images

Compile the project to a docker image to run in a container:

```bash
npm run build:dev
```

### 5. Start the containers

Start the containers with the images form the last step:

```bash
npm run dev
```

If it gives an replication error please wait a bit before running the command again this should resolve the problem

### 6. Initialise data

Inserts the data from the seed file into the database:

```bash
npm run seed
```

---

## Frontend guide

The first page you are greeted with when opening the app is the login page. Please use the credentials provided to you by your system administrator.
<br/>
<img src="/.github/image/Login.png" alt="This is the login screen" title="This is the login screen" height="400px" />
<img src="/.github/image/LoginDark.png" alt="This is the login dark screen" title="This is the login dark screen" height="400px" />

If you cannot login please contact the administrator to resolve the problem.

To navigate between the different pages in the app, use the nav bar at the bottom of the screen. Down here there will be up to 4 icons depending on the access permissions that the administrator gave you.
<br/>

### Available Main Pages:

1. **Profile**
2. **Admin**
3. **User**
2. **Stats**
3. **Device**

### Profile Page:

Coming soon

#### Change Password Form

Here you can change the password for your account. Simply enter the old password and the new one you would like. It will ask you to confirm the new password. If you press **Change**, your password has then been changed. In case of any error, please follow the on-screen instructions.
<br/>

### Admin Page (Coming soon):

The Management page allows you to manage users, menus, stats, and storage. Depending on your permissions, you may have access to different sections of this page.
<br/>

#### Users Page (Coming soon)

In the **Users** page, you can view and manage all the users of the system. You can add new users, edit existing users, or deactivate users.

**IMPORTANT**: To set the password for a user, whether creating or editing, use the **Set Password** button. A form will pop up where you can set the password.
<br/>

##### Create User (Coming soon)

To create a new user press the plus icon at the bottom, fill out the form with the required details. After submitting the form, the new user will be created and added to the system.
<br/>

##### Edit User (Coming soon)

On the **Edit User** page, you can modify the details of an existing user. Simply search for the user you want to edit and press on it, update the required fields, and save the changes.
<br/>

#### Stats Page (Coming soon)

The **Stats** page displays important statistics related to the climate. Here, you can view metrics such as temperature, humanity, light and other key data.
<br/>

#### Device Page (Coming soon)

The **Device** page allows you to manage the devices. You can add new devices or change the frequency or delete existing ones to keep track of the devices.
<br/>

##### Create Devices (Coming soon)

To create a new device press the plus icon, enter the required details such as item name, unit, and quantity. Submit the form to add the new item to the inventory.
<br/>

##### Edit Device (Coming soon)

On the **Edit Device**, you can modify the details of an existing stock item. Search for the item you want to update and swipe from right to left on it and press the edit icon, make the necessary changes, and save the updated information.
<br/>

##### Delete Device (Coming soon)

To delete a device swipe from left to right on it and then press the delete icon and confirm your action
<br/>

Press **Submit** to place the order.
