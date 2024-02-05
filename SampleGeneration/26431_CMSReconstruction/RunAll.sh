#!/bin/bash

echo Please set up CMSSW 13_0_3 for the production

cmsRun PPD-HINPbPbSpring23GS-00001_1_cfg.py
cmsRun HIN-HINPbPbSpring23Digi-00001_1_cfg.py
cmsRun HIN-HINPbPbSpring23Reco-00001_1_cfg.py
cmsRun HIN-HINPbPbSpring23MiniAOD-00001_1_cfg.py
