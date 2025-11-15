# Weather Widget

A lightweight desktop weather application built with Qt 6 and C++. Intended to be used like the Windows widget, but exclusively for weather data. Displays current temperature and a variety of other data upon hover.

## Features

- Real-time weather data for any location
- Automatic location detection via IP geolocation
- Manual city search
- Clean, minimal interface
- Low resource usage

## Tech Stack

- **Language:** C++
- **Framework:** Qt 6 (Widgets)
- **API:** OpenWeatherMap API
- **HTTP:** QNetworkAccessManager

## Building from Source

### Prerequisites
- Qt 6.x
- C++ compiler (MinGW, MSVC, or GCC)
- CMake or qmake

### Build Instructions

1. Clone the repository:
```bash
git clone https://github.com/trumpetkern27/weather.git
cd weather
```

2. Open `weather.pro` in Qt Creator

3. Build and run:
   - Switch to Release mode
   - Build > Build All
   - Run

## Usage

**Automatic Location:**
- On startup, the app will ask if you would like for it to attempt to detect your location automatically.

**Manual Search:**
- Enter a city name in the search bar
- Press Enter or click Search

**Running on Startup (Windows):**
1. Press `Win + R`, type `shell:startup`, press Enter
2. Create a shortcut to `weather.exe` in the startup folder

## Project Structure

```
weather/
├── main.cpp              # Application entry point
├── mainwindow.cpp/h      # Main UI logic
├── weatherservice.cpp/h  # API communication
├── mainwindow.ui         # Qt Designer UI file
└── weather.pro           # Qt project file
```

## Known Limitations

- Windows only (currently)
- Requires internet connection

## Future Improvements

- Environment variable for API key
- Customizable refresh intervals
- Extended forecast options
- Better UI

## License

MIT License - see LICENSE file for details

## Acknowledgments

- Weather data provided by [OpenWeatherMap](https://openweathermap.org/)
- Location detection via [ip-api.com](http://ip-api.com/)