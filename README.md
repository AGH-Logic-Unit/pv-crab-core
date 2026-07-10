# Pulsar-V: Crab Core


## Workplace setup

#### Start devcontainer:
You can use VS Code extension or DevPod:
```bash
devpod up .
```

#### Generate filelist
```bash
python scripts/gen_filelist.py -o verible.filelist
```

## Running test

#### Generate filelist
```bash
python scripts/gen_filelist.py -o output/run.filelist
```
You can use global filelist or predefined one

#### Run testlist
```bash
python -m pytest scripts/test.py --testlist <path>
```

#### Run testlist with specified filelist
```bash
python -m pytest scripts/test.py --testlist <path> --filelist <path>
```
You can also predefine filelist in testlist config or test case
