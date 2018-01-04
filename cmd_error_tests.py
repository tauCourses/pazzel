from subprocess import Popen, PIPE
import time

error_messgaes = {'wrongNumberOfArguments': 'Wrong number of command line arguments\n',
                  'unableToOpenInputFile': 'Unable to open input file\n',
                  'notValidNumberOfThreads': 'Number of threads is not valid\n',
                  'valid': ''}

error_tests = [('a b -ro', 'wrongNumberOfArguments'),
                ('a b -thread 1', 'wrongNumberOfArguments'),
                ('asdfdsa b', 'unableToOpenInputFile'),
                ('a -threads b', 'notValidNumberOfThreads'),
                ('a b -threads', 'notValidNumberOfThreads'),
                ('a -rotate b -threads 0', 'notValidNumberOfThreads'),
                ('-threads 2 a -rotate b ', 'valid'),
                ('a b -threads 2 -rotate', 'valid'),]

for test in error_tests:
    p = Popen(['./ex3'] + test[0].split(), stdin=PIPE, stdout=PIPE, stderr=PIPE)
    time.sleep(0.1)
    out, err = p.communicate()
    if err.decode('ascii') != error_messgaes[test[1]]:
        print('for %s accepted:\n%s\nBut got:\n%s' % (test[0],error_messgaes[test[1]],err.decode('ascii')))
    else:
        print('pass')