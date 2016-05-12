#!/usr/bin/env python
# -*- coding: utf-8 -*-

from setuptools import setup, find_packages

PACKAGE = "patlms-web"

setup(
    name=PACKAGE,
    version=0.1,
    description='PAT LMS web server',
    author='Adam Chyła',
    author_email='adam@chyla.org',
    license='GPLv3',
    url='https://github.com/chyla/pat-lms',
    packages=find_packages(exclude=['tests.*', 'tests', 'test.*', 'test*']),

    install_requires=['Django>=1.8.0',
                      ],
    setup_requires=['pytest-runner',
                    ],
    tests_require=['pytest',
                   ],
    classifiers=[
        'Environment :: Web Environment',
        'Operating System :: OS Independent',
        'Programming Language :: Python',
        'Framework :: Django',
    ],
    zip_safe=False,
)