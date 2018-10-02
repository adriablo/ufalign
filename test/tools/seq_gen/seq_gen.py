"""
# Load Python environment:
source activate bio
"""

import random


from Bio.Seq import Seq
from Bio import SeqIO
from Bio.SeqRecord import SeqRecord


def load_seq(f_name, s_id):
    for sr in SeqIO.parse(f_name, 'fasta'):
        if sr.id == s_id:
            return sr
    return None


def gen_seq(s_rec, s_len, f_name, n=100):
    s = s_rec.seq
    rs_list = []
    i = 0
    while i < n:
        pos = random.randint(0, len(s) - s_len)
        rs = s[pos:(pos + s_len)]
        if rs.find('N') > -1:
            continue
        rs_list.append(SeqRecord(rs, id = 'q{:06d}'.format(i), description=''))
        i += 1
    SeqIO.write(rs_list, f_name, 'fasta')
