# Star Rod Mod template

This is a template repository for making Paper Mario mods using [Star Rod](https://github.com/nanaian/star-rod).

### Getting started

* [Generate a new repository using this template](https://github.com/nanaian/star-rod-mod/generate)
* Clone your new repository
* Open the [Star Rod](https://github.com/nanaian/star-rod/releases/latest) Mod Manager
* Set the _Mod Folder_ to the directory you cloned into
* _Copy Assets to Mod_
* Create your mod as usual, e.g. make a change and compile the mod.

### Images, sprites, and audio

Some assets, such as those in the `image` and `sprite` folders, do not use the `patch`/`src` system. By default, this template **ignores all changes to assets within these folders**, because you shouldn't be including content from the original ROM in any public Git repository. You'll need to explicitly include assets you change by editing the relevant `.gitignore` file.

For example, assume I have changed `image/bg/kpa_bg.png`. In order to ask Git to include this change, I add the following line to `image/.gitignore`:

```diff
  # Original game data
  *.png
  *.mis
  *.iis
  *.mtl
  *.txa
+
+ # Modified assets
+ !/bg/kpa_bg.png
```

Git will now track changes to this file now and in the future.

(Alternatively, I could have used `git add --force image/bg/kpa_bg.png`. This will work, but [it may cause issues](https://stackoverflow.com/a/42723384) in the future if the file is deleted, for whatever reason, but then recreated later. Git will have forgotten that it should be tracking the file!)

This process applies to any assets in the `audio`, `image`, and `sprite` folders.
