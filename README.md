# carpark

i should have added a src folder - Ethan


Check == attempted

# Simulator
 -  Intalise memory (Check)
 -  Init other mutex (TODO)
 -  Generate a car 1-100ms (Check)
 -  Car logic (TODO)
    - Add car queue (TODO)
    - Send car to entrance boom gate (Check)
    - Send license to LPR (Check)
    - Wait for manager to display info sign (TODO)
    - Wait for boomgate to open (TODO)
    - Drive to level {Trigger Level LPR} (TODO)
    - Park to set time (TODO)
    - Exit park on set level and trigger exit LPR (TODO)
    - Wait for exit boomgate
    - Die

# Manager 
- Load shared memory (Check)
- Create LPR threads (Check)
    - Entrace LPR
        - Read plate of car (Check)
        - Display parking level on info sign (TODO)
        - open entrance boom gate   (TODO)
        - close entrance boom gate  (TODO)
    - Level LPR
        - BRR idk trigger parked    (TODO)
    - Exit LPR
        - Reads plate   (TODO)
        - Opens/Close boomgate(TODO)
        - Free the parking spot (TODO)
        - Trigger billing(TODO)
- Hash table
    - Check validity of plate (TODO)
    - Reject invalid car    (TODO)
- Manage carspaces 
    - Send cars to free parking levels(TODO)
    - Reject car if full (TODO)
- Billing 
    - Charge for each car   (TODO)
    - Total revenue (TODO)

- UI
    - Display info (Check)


# Fire alarm




    ->>>>> Car enters 
    ->>>>> LPR
    ->>>>> wait for infoSign 
    --------> Decide if car can go in and where to go
    ->>>>> Car waits for the boom 
    ->>>>> Car drives to level