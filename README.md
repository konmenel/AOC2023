# Advent of Code 2023
This is just a repo with my solutions for the Advent of Code 2023 challenges.

## To create new day from template
For a given day "**XX**" do one of the following

Run the powershell script
```powershell
.\setup.ps1 XX
```
or the bash script
```bash
$ ./setup.sh XX
```

## To download the inputs
The custom python script `getinput.py` downloads the puzzle inputs
and example inputs. 

### Dependencies
* requests
* Beautiful Soup 4

### Setup

Before running the script you need to setup the `cookie.json` file. This needs to be done because the inputs differ for each user. Simply copy the value for session cookie from https://adventofcode.com/ using the inspect tool in the browser, to the `cookie_example.json` file and rename to `cookie.json`.

### Download
After the above are done, for a given day "**XX**" you should be able to excecute:

```bash
$ python3 getinput.py XX
```

## To build and run a solution
For a given day "**XX**" do one of the following

### Debug
```bash
$ ./runday XX debug
```

### Release
```bash
$ ./runday XX release
```

or just
```bash
$ ./runday XX
```
