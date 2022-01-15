#!/bin/python
# Updates all icons in the application from github.com/KDE/breeze-icons
import enum
import filecmp
import os
import pathlib
import shutil
import subprocess
import sys
import tempfile
import typing

import requests

REPO: typing.Final[str] = "KDE/breeze-icons"

with open("version", "r") as f:
	current = f.read()

latest: str = requests.get(f"https://api.github.com/repos/{REPO}/tags").json()[0]["name"]
if latest == current and "--force" not in sys.argv:
	print("Already up-to-date")
	exit(0)

print(f"Latest version is {latest}, you have {current}")

repo_dir = os.path.join(tempfile.gettempdir(), f"{os.path.basename(REPO)}-{latest}")
if not os.path.isdir(repo_dir):
	subprocess.run([
		"git",
		"clone",
		"--depth", "1",
		"--branch", latest,
		f"https://github.com/{REPO}.git",
		repo_dir,
	], stdout=subprocess.DEVNULL,
		stderr=subprocess.DEVNULL)

icon_folders = [
	"actions",
	"status",
	"places",
	"devices",
	"emblems",
]


class IconType(enum.Enum):
	LIGHT = 0
	DARK = 1

	def source_name(self) -> str:
		if self == IconType.LIGHT:
			return "icons"
		if self == IconType.DARK:
			return "icons-dark"

	def target_name(self) -> str:
		if self == IconType.LIGHT:
			return "light"
		if self == IconType.DARK:
			return "dark"


def find_icon(folder_name: str, icon_name: str) -> typing.Optional[str]:
	icon_size = "symbolic" if pathlib.Path(icon_name).stem.endswith("-symbolic") else "22"
	for folder in icon_folders:
		path = os.path.join(repo_dir, folder_name, folder, icon_size, icon_name)
		if os.path.isfile(path):
			return path
	return None


def copy_icon(icon_name: str, icon_type: IconType) -> bool:
	source_path = find_icon(icon_type.source_name(), icon_name)
	if source_path is None and icon_type == IconType.DARK:
		source_path = find_icon(IconType.LIGHT.source_name(), icon_name)
	if source_path is None:
		print(f"Error: failed to find source for {icon_type.source_name()}/{filename}")
		return False
	target_path = os.path.join(os.getcwd(), icon_type.target_name(), icon_name)
	if filecmp.cmp(source_path, target_path):
		return False
	shutil.copy(source_path, target_path)
	return True


updates = 0
for filename in os.listdir(os.path.join(os.getcwd(), "dark")):
	if copy_icon(filename, IconType.LIGHT):
		updates += 1
	if copy_icon(filename, IconType.DARK):
		updates += 1

print("%i %s updated" % (updates, "icon" if updates == 1 else "icons"))

with open("version", "w") as f:
	f.write(latest)
