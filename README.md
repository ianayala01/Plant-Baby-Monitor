# Plant-Baby-Monitor
This project is intended to track daily light exposure and average temperatures for growing plants. 
It uses a cicrular buffer and contains a maximum of 10 days at a time, but that can easily be modified by changing the MAX_DAYS macro. 
Individual days are tracked based on a light threshold that has been calibrated to my LED grow lights. If the plants are in the dark for a set amount of time, that marks the end of the day and environmental stats are saved.
at the press of a button, these stats are formatted and sent over email with the actual dates on which the stats were collected.

# future updates:
- switch to using preferences.h for persistent credential and statistics storage.
