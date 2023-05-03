from setuptools import setup

setup(
    name='neutral_atom_imaging_simulation',
    version='0.0.1',    
    description='Package for generating simulated images of neutral atoms',
    url='https://gitlab.lrz.de/jonas.winklmann/neutral-atom-imaging-simulation',
    author='Jonas Winklmann',
    author_email='jonas.winklmann@tum.de',
    packages=['neutral_atom_imaging_simulation'],
    install_requires=['matplotlib',
                      'numpy'                  
                      ],
    license_files='LICENSE.txt',
    classifiers=[
        'Development Status :: 3 - Alpha',
        'Intended Audience :: Science/Research',
        'License :: OSI Approved :: MIT License',  
        'Operating System :: POSIX :: Linux',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.4',
        'Programming Language :: Python :: 3.5',
    ],
    include_package_data=True,
    package_data={'': ['lib/*.so','lib/*.dll']}
)