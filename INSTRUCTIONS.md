# Project Setup Instructions

Follow these steps to set up the project:

## 1. Install SUMO
- Download and install SUMO from the official website.
- Ensure SUMO is installed in the following directory:  
    `C:\Program Files (x86)\Eclipse\Sumo`.

## 2. Set Environment Variable
- Set the `SUMO_HOME` environment variable to:  
    `C:\Program Files (x86)\Eclipse\Sumo`.

## 3. Prepare the SUMO Folder
- Clone or download this repository.
- Replace the file `C:\sumo\tools\osmWebWizard.py` with the `osmWebWizard.py` file provided in this repository.

## 4. Install Python Requirements
- Ensure Python is installed on your system.
- Navigate to the project directory and install the required Python packages using:  
    ```bash
    pip install -r requirements.txt
    ```

## 5. Install Bun
- Install Bun by following the instructions on the [Bun website](https://bun.sh/).
- Use Bun to install Node.js dependencies:  
    ```bash
    bun install
    ```

## 6. Run the Applications
- Start the Express server:  
    ```bash
    bun run server
    ```
- Start the Nuxt application:  
    ```bash
    bun run dev
    ```

## 7. Configure Environment Variables
- Obtain the `.env` file from the repository owner.
- Place the `.env` file in the root directory of the project.

You are now ready to use the project!