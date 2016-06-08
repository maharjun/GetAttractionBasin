#!/usr/bin/env python3.5

import sys
import os
from RepoManagement.BasicUtils import changedDir, getFrameDir


def SetupDynSystem(DynSystemHPPPath=None):

    if not DynSystemHPPPath:
        DynSystemHPPPath = ''

    # get Current Script Directory
    CurrentScriptDir = getFrameDir()

    # Initialize Directories relevant to CMake
    CMakeModulesDir = os.path.join(CurrentScriptDir, 'CMakeModules')
    CurrentSourceDir = CurrentScriptDir
    BuildDir = os.path.join(os.getcwd(), 'GetAttBasin_build')

    # Add CMakeModulesDir to sys path to get relevant functions
    sys.path.insert(0, CMakeModulesDir)
    from CMakePyHelper import getDefaultPlatformGen
    from CMakePyHelper import CMakeGenCall, CMakeBuildCall

    DefaultGenerator = getDefaultPlatformGen()

    # Initialize Path to the HPP file correspondingto the dynamic system.
    DynSystemHPPPath = os.path.join(os.getcwd(), DynSystemHPPPath)
    
    # generate and build
    if not os.path.isdir(BuildDir):
        os.mkdir(BuildDir)
    with changedDir(BuildDir):
        isCMakeGenSuccess = CMakeGenCall(CurrentSourceDir,
                                         Generator=DefaultGenerator,
                                         BuildConfig='Release',
                                         Silent=True,
                                         DYN_SYSTEM_HPP_PATH=DynSystemHPPPath)
    if isCMakeGenSuccess:
        CMakeBuildSuccess = CMakeBuildCall(BuildDir,
                                           Target='install',
                                           BuildConfig='Release')

    return CMakeBuildSuccess


if __name__ == "__main__":
    argc = len(sys.argv)
    argv = sys.argv

    if argc == 1:
        DynSystemHPPPath = None
    elif argc == 2:
        DynSystemHPPPath = argv[1]
    else:
        print("Incorrect number of arguments expected at max 1 argument")

    SetupDynSystem(DynSystemHPPPath)
