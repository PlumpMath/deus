# Git
Before pushing a new library to the server, the target directory must be initialized.

```sh
git init --bare
```

## Library
Initialize the library and upload it to the server.

```sh
git init
git add .
git commit -m "first commit"
git remote add origin "//gate/git/third_party/name"
git push -u origin master
```

## Project
Initialize the project and upload it to the server.

```sh
git init
git add .gitignore
git commit -m "first commit"
git checkout -b devel
git submodule add "//gate/git/third_party/name"
git add .
git commit -m "first commit"
git push -u origin master
git push -u origin devel
```

## Branch
```sh
git checkout -b devel
```

## Project
```sh
git init
git add .
git commit -m "first commit"
git remote add origin "//gate/git/third_party/name"
git push -u origin master
```

## Submodule
```sh

```