# Project Title

WSNReliability - diploma project, methods to calc WSN reliability

## Getting Started

You need a pre-installed Brew util using MAC OS.

### Prerequisites

On MAC OS:

```
brew install llvm
brew install boost
brew install libomp
brew install opencv
brew install RapidJSON
brew install graphviz
```

### Installing

Use *brew install %name%* commands mentioned in Prerequisites.

Set Configs - 'config/config.txt'

```
config.txt
accuracy = 100
oImgSizeX = 300
oImgSizeY = 300
oImgScale = 1
oImgFormat = png
numThreads = 32

```

Set Input files - 'input/%filename%.json'
```
{
  "name": "Calambus",
  "author": "Alexander Kucherov",
  "date": "13.3.2018",

  "nodes": [
    {
      "id": 0,
      "coordinates": [11.120, 28.233],
      "relations": [1, 2, 3],
      "reliability": 1,
      "coverage": 1,
      "stock": true
    },

    {
      "id": 1,
      "coordinates": [14.120, 25.233],
      "relations": [0],
      "reliability": 0.9,
      "coverage": 1,
      "stock": false
    },

    {
      "id": 2,
      "coordinates": [58.196, 28.547],
      "relations": [0, 4],
      "reliability": 0.9,
      "coverage": 1.5,
      "stock": false
    },

    {
      "id": 3,
      "coordinates": [192.196, 5.247],
      "relations": [0, 4],
      "reliability": 0.9,
      "coverage": 2,
      "stock": false
    },

    {
      "id": 4,
      "coordinates": [5.196, 55.247],
      "relations": [3,2],
      "reliability": 0.9,
      "coverage": 0.5,
      "stock": false
    }
  ]
}
```

End with an example of getting some data out of the system or using it for a little demo

## Running the tests

No Tests yet.

## Deployment

Build using cmake and simply run ./WSNReliability

## Built With

* [CMake](https://cmake.org/documentation/) - Cross-platform application builder used

## Contributing

Please read [CONTRIBUTING.md](https://gist.github.com/PurpleBooth/b24679402957c63ec426) for details on our code of conduct, and the process for submitting pull requests to us.

## Versioning

No versioning yet as project is in alpha.

## Authors

* **Alexander Kucherov** - *Initial work* - [Zinkutal](https://github.com/Zinkutal)

See also the list of [contributors](https://github.com/WSNReliability/contributors) who participated in this project.

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments

Denis Migov for consultations