import os
import xml.etree.ElementTree as ET

def get_percentage_test_passed():
    tree = ET.parse('../../output/results.xml')
    root = tree.getroot()
    testsuite_elm = root[0]
    tests_number = int(testsuite_elm.get('tests'))
    tests_failed = int(testsuite_elm.get('failures'))
    tests_error = int(testsuite_elm.get('errors'))
    tests_skipped = int(testsuite_elm.get('skipped'))
    
    tests_passed = tests_number - (tests_failed + tests_error + tests_skipped)

    return int((tests_passed /tests_number) * 100)

def write_percentage_into_file(percentage):
    file = os.getenv('OUTPUT_FILE')
    if file is None or len(file) == 0:
        if not os.path.isdir('../../output'):
            os.mkdir('../../output')
        file = '../../output/tests_result.txt'
    with open(file, 'w') as f:
        f.write(str(percentage))

if __name__ == '__main__':
    percentage = get_percentage_test_passed()
    write_percentage_into_file(percentage)
