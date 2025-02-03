import pysvs as ps
import numpy as np
import math
import math 
import os

# dataset_path = '/home/saminyeaser/OSU study/Research-Implementation/dataset/'
dataset_path = '/data/kabir/similarity-search/dataset/'

def read_vecs(filePath):
    return ps.read_vecs(dataset_path + filePath)

def get_data_gist(m=None):
    # data we will search through
    xb = read_vecs('gist/gist_base.fvecs')  # 1M samples
    # also get some query vectors to search with
    xq = read_vecs('gist/gist_query.fvecs')
    # take just one query (there are many in sift_learn.fvecs)
    gt = read_vecs('gist/gist_groundtruth.ivecs')
    return xb,xq,gt

def get_data_sift(m=None):
    # data we will search through
    xb = read_vecs('siftsmall/siftsmall_base.fvecs')  # 1M samples
    # also get some query vectors to search with
    xq = read_vecs('siftsmall/siftsmall_query.fvecs')
    # take just one query (there are many in sift_learn.fvecs)
    gt = read_vecs('siftsmall/siftsmall_groundtruth.ivecs')
    return xb,xq,gt

def get_data_sift1M(m=None):
    # data we will search through
    xb = read_vecs('sift/sift_base.fvecs')  # 1M samples
    # also get some query vectors to search with
    xq = read_vecs('sift/sift_query.fvecs')
    # take just one query (there are many in sift_learn.fvecs)
    gt = read_vecs('sift/sift_groundtruth.ivecs')
    return xb,xq,gt

def get_data_glove(m=None):
    # data we will search through
    xb = read_vecs('glove/base.fvecs')  # 1M samples
    # also get some query vectors to search with
    xq = read_vecs('glove/query.fvecs')
    # take just one query (there are many in sift_learn.fvecs)
    gt = read_vecs('glove/groundtruth.ivecs')
    return xb,xq,gt

def get_data_imageNet(m=None):
    # data we will search through
    xb = read_vecs('imageNet/base.fvecs')  # 1M samples
    # also get some query vectors to search with
    xq = read_vecs('imageNet/query.fvecs')
    # take just one query (there are many in sift_learn.fvecs)
    gt = read_vecs('imageNet/groundtruth.ivecs')
    return xb,xq,gt

def get_data_notre(m=None):
    # data we will search through
    xb = read_vecs('notre/base.fvecs')  # 1M samples
    # also get some query vectors to search with
    xq = read_vecs('notre/query.fvecs')
    # take just one query (there are many in sift_learn.fvecs)
    gt = read_vecs('notre/groundtruth.ivecs')
    return xb,xq,gt

def get_data_ukbench(m=None):
    # data we will search through
    xb = read_vecs('ukbench/base.fvecs')  # 1M samples
    # also get some query vectors to search with
    xq = read_vecs('ukbench/query.fvecs')
    # take just one query (there are many in sift_learn.fvecs)
    gt = read_vecs('ukbench/groundtruth.ivecs')
    return xb,xq,gt

def get_data_crawl(m=None):
    # data we will search through
    xb = read_vecs('crawl/base.fvecs')  # 1M samples
    # also get some query vectors to search with
    xq = read_vecs('crawl/query.fvecs')
    # take just one query (there are many in sift_learn.fvecs)
    gt = read_vecs('crawl/groundtruth.ivecs')
    return xb,xq,gt



def get_data_audio(m=None):
    # data we will search through
    xb = read_vecs('audio/audio_base.fvecs')  # 1M samples
    # also get some query vectors to search with
    xq = read_vecs('audio/audio_query.fvecs')
    # take just one query (there are many in sift_learn.fvecs)
    gt = read_vecs('audio/audio_groundtruth.ivecs')
    return xb,xq,gt

def get_data_cifar(m=None):
    # data we will search through
    xb = read_vecs('cifar/base.fvecs')  # 1M samples
    # also get some query vectors to search with
    xq = read_vecs('cifar/query.fvecs')
    # take just one query (there are many in sift_learn.fvecs)
    gt = read_vecs('cifar/groundtruth.ivecs')
    return xb,xq,gt

def extend_time_series_with_zeros(arr, dim):
    new_arr = np.zeros([arr.shape[0],dim])
    for i in range(arr.shape[0]):
        for j in range(arr.shape[1]):
            new_arr[i][j]=arr[i][j]
    return new_arr
    

def get_data_enron(m=None):
    # data we will search through
    xb = read_vecs('enron/base.fvecs')  # 1M samples
    # also get some query vectors to search with
    xq = read_vecs('enron/query.fvecs')
    # take just one query (there are many in sift_learn.fvecs)
    gt = read_vecs('enron/groundtruth.ivecs')
    return xb,xq,gt






def get_data_millionSong(m=None):
    # data we will search through
    xb = read_vecs('millionSong/base.fvecs')  # 1M samples
    # also get some query vectors to search with
    xq = read_vecs('millionSong/query.fvecs')
    # take just one query (there are many in sift_learn.fvecs)
    gt = read_vecs('millionSong/groundtruth.ivecs')
    return xb,xq,gt

def get_data_MNIST(m=None):
    # data we will search through
    xb = read_vecs('MNIST/base.fvecs')  # 1M samples
    # also get some query vectors to search with
    xq = read_vecs('MNIST/query.fvecs')
    # take just one query (there are many in sift_learn.fvecs)
    gt = read_vecs('MNIST/groundtruth.ivecs')
    return xb,xq,gt

def get_data_nuswide(m=None):
    # data we will search through
    xb = read_vecs('nuswide/base.fvecs')  # 1M samples
    # also get some query vectors to search with
    xq = read_vecs('nuswide/query.fvecs')
    # take just one query (there are many in sift_learn.fvecs)
    gt = read_vecs('nuswide/groundtruth.ivecs')
    return xb,xq,gt



def get_data_sun(m=None):
    # data we will search through
    xb = read_vecs('sun/base.fvecs')  # 1M samples
    # also get some query vectors to search with
    xq = read_vecs('sun/query.fvecs')
    # take just one query (there are many in sift_learn.fvecs)
    gt = read_vecs('sun/groundtruth.ivecs')
    return xb,xq,gt

def get_data_deep1M(m=None):
    # data we will search through
    xb = read_vecs('deep/base.fvecs')  # 1M samples
    # also get some query vectors to search with
    xq = read_vecs('deep/query.fvecs')
    # take just one query (there are many in sift_learn.fvecs)
    gt = read_vecs('deep/groundtruth.ivecs')
    return xb,xq,gt


def get_data_trevi(m=None):
    # data we will search through
    xb = read_vecs('trevi/base.fvecs')  # 1M samples
    # also get some query vectors to search with
    xq = read_vecs('trevi/query.fvecs')
    # take just one query (there are many in sift_learn.fvecs)
    gt = read_vecs('trevi/groundtruth.ivecs')
    return xb,xq,gt


def get_data_uqv(m=None):
    # data we will search through
    xb = read_vecs('uqv/base.fvecs')  # 1M samples
    # also get some query vectors to search with
    xq = read_vecs('uqv/query.fvecs')
    # take just one query (there are many in sift_learn.fvecs)
    gt = read_vecs('uqv/groundtruth.ivecs')
    return xb,xq,gt


def get_data_nytimes(m=None):
    # data we will search through
    xb = read_vecs('nytimes/base.fvecs')  # 1M samples
    # also get some query vectors to search with
    xq = read_vecs('nytimes/query.fvecs')
    # take just one query (there are many in sift_learn.fvecs)
    gt = read_vecs('nytimes/groundtruth.ivecs')
    return xb,xq,gt

def get_data_lastfm(m=None):
    # data we will search through
    xb = read_vecs('lastfm/base.fvecs')  # 1M samples
    # also get some query vectors to search with
    xq = read_vecs('lastfm/query.fvecs')
    # take just one query (there are many in sift_learn.fvecs)
    gt = read_vecs('lastfm/groundtruth.ivecs')
    return xb,xq,gt

def get_data_astro1m(m=None):
    # data we will search through
    xb = read_vecs('astro1m/base.fvecs')  # 1M samples
    # also get some query vectors to search with
    xq = read_vecs('astro1m/query.fvecs')
    # take just one query (there are many in sift_learn.fvecs)
    gt = read_vecs('astro1m/groundtruth.ivecs')
    return xb,xq,gt

def get_data_seismic1m(m=None):
    # data we will search through
    xb = read_vecs('seismic1m/base.fvecs')  # 1M samples
    # also get some query vectors to search with
    xq = read_vecs('seismic1m/query.fvecs')
    # take just one query (there are many in sift_learn.fvecs)
    gt = read_vecs('seismic1m/groundtruth.ivecs')
    return xb,xq,gt

def get_data_sald1m(m=None):
    # data we will search through
    xb = read_vecs('sald1m/base.fvecs')  # 1M samples
    # also get some query vectors to search with
    xq = read_vecs('sald1m/query.fvecs')
    # take just one query (there are many in sift_learn.fvecs)
    gt = read_vecs('sald1m/groundtruth.ivecs')
    return xb,xq,gt


def get_data_bigann(m=None):
    # data we will search through
    xb = read_vecs('bigann/base.fvecs')  # 1M samples
    # also get some query vectors to search with
    xq = read_vecs('bigann/query.fvecs')
    # take just one query (there are many in sift_learn.fvecs)
    gt = read_vecs('bigann/groundtruth.ivecs')
    return xb,xq,gt


def get_data_space1V(m=None):
    # data we will search through
    xb = read_vecs('space1V/base.fvecs')  # 1M samples
    # also get some query vectors to search with
    xq = read_vecs('space1V/query.fvecs')
    # take just one query (there are many in sift_learn.fvecs)
    gt = read_vecs('space1V/groundtruth.ivecs')
    return xb,xq,gt




def get_data_text_to_image(m=None):
    # data we will search through
    xb = read_vecs('text-to-image/base.fvecs')  # 1M samples
    # also get some query vectors to search with
    xq = read_vecs('text-to-image/query.fvecs')
    # take just one query (there are many in sift_learn.fvecs)
    gt = read_vecs('text-to-image/groundtruth.ivecs')
    return xb,xq,gt




def get_data_random(m=None):
    # data we will search through
    xb = read_vecs('random/base.fvecs')  # 1M samples
    # also get some query vectors to search with
    xq = read_vecs('random/query.fvecs')
    # take just one query (there are many in sift_learn.fvecs)
    gt = read_vecs('random/groundtruth.ivecs')
    return xb,xq,gt

def get_data_movielens(m=None):
    # data we will search through
    xb = read_vecs('movielens/base.fvecs')  # 1M samples
    # also get some query vectors to search with
    xq = read_vecs('movielens/query.fvecs')
    # take just one query (there are many in sift_learn.fvecs)
    gt = read_vecs('movielens/groundtruth.ivecs')
    return xb,xq,gt

def get_data_netflix(m=None):
    # data we will search through
    xb = read_vecs('netflix/base.fvecs')  # 1M samples
    # also get some query vectors to search with
    xq = read_vecs('netflix/query.fvecs')
    # take just one query (there are many in sift_learn.fvecs)
    gt = read_vecs('netflix/groundtruth.ivecs')
    return xb,xq,gt



def get_data_tiny5m(m=None):
    # data we will search through
    xb = read_vecs('tiny5m/base.fvecs')  # 1M samples
    # also get some query vectors to search with
    xq = read_vecs('tiny5m/query.fvecs')
    # take just one query (there are many in sift_learn.fvecs)
    gt = read_vecs('tiny5m/groundtruth.ivecs')
    return xb,xq,gt



def get_data_word2vec(m=None):
    # data we will search through
    xb = read_vecs('word2vec/base.fvecs')  # 1M samples
    # also get some query vectors to search with
    xq = read_vecs('word2vec/query.fvecs')
    # take just one query (there are many in sift_learn.fvecs)
    gt = read_vecs('word2vec/groundtruth.ivecs')
    return xb,xq,gt
    return xb,xq,gt



def get_data_yahoomusic(m=None):
    # data we will search through
    xb = read_vecs('yahoomusic/base.fvecs')  # 1M samples
    # also get some query vectors to search with
    xq = read_vecs('yahoomusic/query.fvecs')
    # take just one query (there are many in sift_learn.fvecs)
    gt = read_vecs('yahoomusic/groundtruth.ivecs')
    return xb,xq,gt


data_src_lvq_train = {
    'siftsm': '/data/kabir/similarity-search/dataset/siftsmall/siftsmall_base.fvecs',
    'sift1m': '/data/kabir/similarity-search/dataset/sift/sift_base.fvecs',
    'gist': '/data/kabir/similarity-search/dataset/gist/gist_base.fvecs',
    'notre': '/data/kabir/similarity-search/dataset/notre/base.fvecs',
    'ukbench': '/data/kabir/similarity-search/dataset/ukbench/base.fvecs',
    'audio': '/data/kabir/similarity-search/dataset/audio/audio_base.fvecs',
    'cifar': '/data/kabir/similarity-search/dataset/cifar/base.fvecs',
    'crawl': '/data/kabir/similarity-search/dataset/crawl/base.fvecs',
    'enron': '/data/kabir/similarity-search/dataset/enron/base.fvecs',
    'millionSong': '/data/kabir/similarity-search/dataset/millionSong/base.fvecs',
    'MNIST': '/data/kabir/similarity-search/dataset/MNIST/base.fvecs',
    'nuswide': '/data/kabir/similarity-search/dataset/nuswide/base.fvecs',
    'sun': '/data/kabir/similarity-search/dataset/sun/base.fvecs',
    'deep1m': '/data/kabir/similarity-search/dataset/deep/base.fvecs',
    'trevi': '/data/kabir/similarity-search/dataset/trevi/base.fvecs',
    'uqv': '/data/kabir/similarity-search/dataset/uqv/base.fvecs',
    'nytimes': '/data/kabir/similarity-search/dataset/nytimes/base.fvecs',
    'lastfm': '/data/kabir/similarity-search/dataset/lastfm/base.fvecs',
    'glove': '/data/kabir/similarity-search/dataset/glove/base.fvecs',
    'imageNet': '/data/kabir/similarity-search/dataset/imageNet/base.fvecs',
    'astro1m': '/data/kabir/similarity-search/dataset/astro1m/base.fvecs',
    'seismic1m': '/data/kabir/similarity-search/dataset/seismic1m/base.fvecs',
    'sald1m': '/data/kabir/similarity-search/dataset/sald1m/base.fvecs',
    'bigann': '/data/kabir/similarity-search/dataset/bigann/base.fvecs',
    'space1V': '/data/kabir/similarity-search/dataset/space1V/base.fvecs',
    'space': '/data/kabir/similarity-search/dataset/space1V/base.fvecs',
    'text-to-image': '/data/kabir/similarity-search/dataset/text-to-image/base.fvecs',
    'random': '/data/kabir/similarity-search/dataset/random/base.fvecs',
    'word2vec': '/data/kabir/similarity-search/dataset/word2vec/base.fvecs',
    'netflix': '/data/kabir/similarity-search/dataset/netflix/base.fvecs',
    'yahoomusic': '/data/kabir/similarity-search/dataset/yahoomusic/base.fvecs',
    'tiny5m': '/data/kabir/similarity-search/dataset/tiny5m/base.fvecs',
}

def get_compressed_loader(relative_path, _dim, data_loader = None):
    if data_loader != None:
        xb,xq,gt = data_loader()
        _dim = xb.shape[1]
    data_loader = ps.VectorDataLoader(
        relative_path,
        ps.DataType.float32,
        dims = _dim    # Passing dim is NOT optional in this case
    )
    padding = _dim
    compressed_loader = ps.LVQ8(
        data_loader,
        padding        # Passing padding is optional.
    )
    return compressed_loader

# xb, xq, gt = get_data_cifar()
# print('cifer', xb.shape, xq.shape, gt.shape)
# xb, xq, gt = get_data_enron()
# print('enron', xb.shape, xq.shape, gt.shape)
# xb, xq, gt = get_data_millionSong()
# print('millionSong', xb.shape, xq.shape, gt.shape)
# xb, xq, gt = get_data_MNIST()
# print('MNIST', xb.shape, xq.shape, gt.shape)
# xb, xq, gt = get_data_nuswide()
# print('nuswide', xb.shape, xq.shape, gt.shape)
# xb, xq, gt = get_data_sun()
# print('sun', xb.shape, xq.shape, gt.shape)


    
# astro1m (1000000, 256) (100, 256) (100, 100)
# audio (53387, 192) (200, 192) (200, 20)
# bigann (1000000, 128) (100, 128) (100, 100)
# cifar (50000, 512) (200, 512) (200, 20)
# crawl (1989995, 300) (10000, 300) (10000, 100)
# deep (1000000, 256) (200, 256) (200, 20)
# enron (94987, 1369) (200, 1369) (200, 20)
# gist (1000000, 960) (1000, 960) (1000, 100)
# glove (1192514, 100) (200, 100) (200, 20)
# imageNet (2340373, 150) (200, 150) (200, 20)
# lastfm (292385, 65) (100, 65) (100, 100)
# millionSong (992272, 420) (200, 420) (200, 20)
# MNIST (69000, 784) (200, 784) (200, 20)
# movielens (10677, 150) (1000, 150) (1000, 100)
# netflix (17770, 300) (1000, 300) (1000, 100)
# notre (332668, 128) (200, 128) (200, 20)
# nuswide (268643, 500) (200, 500) (200, 20)
# nytimes (290000, 256) (100, 256) (100, 100)
# random (1000000, 100) (200, 100) (200, 20)
# sald1m (1000000, 128) (100, 128) (100, 100)
# seismic1m (1000000, 256) (100, 256) (100, 100)
# sift (1000000, 128) (10000, 128) (10000, 100)
# space1V (1000000, 100) (100, 100) (100, 100)
# sun (79106, 512) (200, 512) (200, 20)
# text-to-image (1000000, 200) (100, 200) (100, 100)
# tiny5m (5000000, 384) (1000, 384) (1000, 100)
# trevi (99900, 4096) (200, 4096) (200, 20)
# ukbench (1097907, 128) (200, 128) (200, 20)
# uqv (1000000, 256) (10000, 256) (10000, 100)
# word2vec (1000000, 300) (1000, 300) (1000, 100)
# yahoomusic (136736, 300) (100, 300) (100, 100)

def getDataset(testId):
    if testId.startswith('astro1m'):
        return get_data_astro1m
    if testId.startswith('audio'):
        return get_data_audio
    if testId.startswith('bigann'):
        return get_data_bigann
    if testId.startswith('cifar'):
        return get_data_cifar
    if testId.startswith('crawl'):
        return get_data_crawl
    if testId.startswith('deep'):
        return get_data_deep1M
    if testId.startswith('enron'):
        return get_data_enron
    if testId.startswith('gist'):
        return get_data_gist
    if testId.startswith('glove'):
        return get_data_glove
    if testId.startswith('imageNet'):
        return get_data_imageNet
    if testId.startswith('siftsmall'):
        return get_data_sift
    if testId.startswith('lastfm'):
        return get_data_lastfm
    if testId.startswith('millionSong'):
        return get_data_millionSong
    if testId.startswith('movielens'):
        return get_data_movielens
    if testId.startswith('MNIST'):
        return get_data_MNIST
    if testId.startswith('netflix'):
        return get_data_netflix
    if testId.startswith('notre'):
        return get_data_notre
    if testId.startswith('nuswide'):
        return get_data_nuswide
    if testId.startswith('nytimes'):
        return get_data_nytimes
    if testId.startswith('random'):
        return get_data_random
    if testId.startswith('sald1m'):
        return get_data_sald1m
    if testId.startswith('seismic1m'):
        return get_data_seismic1m
    if testId.startswith('sift'):
        return get_data_sift1M
    if testId.startswith('space1V'):
        return get_data_space1V
    if testId.startswith('sun'):
        return get_data_sun
    if testId.startswith('text-to-image'):
        return get_data_text_to_image
    if testId.startswith('tiny5m'):
        return get_data_tiny5m
    if testId.startswith('trevi'):
        return get_data_trevi
    if testId.startswith('uqv'):
        return get_data_uqv
    if testId.startswith('ukbench'):
        return get_data_ukbench
    if testId.startswith('word2vec'):
        return get_data_word2vec
    if testId.startswith('yahoomusic'):
        return get_data_yahoomusic
    
os.chdir('logs')

all_results={}
for fileName in os.listdir():
    try:
        if not ('build' in fileName):
            testId=fileName.split('.txt')[0]
            # print(testId,'test')
            dataset=getDataset(testId)
            if dataset==None:
                continue
            xb,xq,gt=dataset()
            q=xq.shape[0]
            d=xq.shape[1]
            k=gt.shape[1]
            assert(xq.shape[0]==gt.shape[0])

            results=np.zeros((q,k))
            max_dists=[]
            
            for nq in range(0,q):
                nk=k-1
                k_closest=gt[nq][nk]
                minus = xb[k_closest] - xq[nq]
                distance = np.dot(minus.T, minus)
                max_dists.append(math.sqrt(distance))
            # print(max_dists)
            # print(testId, q, len(max_dists))
            total_count=0
            total_search_time=0.0
            total_indexing_time=0.0
            total_nb=0
            unique_nodes=set()
            with open(fileName, 'r') as file:
                Lines = file.readlines()
                for ln in Lines:
                    if 'Query_exact_distance' in ln:
                        ln=ln.replace('\n','')
                        ln=ln.split('\t')
                        
                        nq=int(ln[3])-1
                        _k=int(ln[4])-1
                        _dist=float(ln[1])
                        unique_nodes.add((nq,_k))
                        if (_k<k and _k>=0 and nq<q and nq>=0):
                            total_nb = total_nb + 1
                            
                        if (_dist<=max_dists[nq] and _k<k and _k>=0 and nq<q and nq>=0):
                            total_count=total_count+1
                    if 'Query_total_time_secs' in ln:
                        ln=ln.replace('\n','')
                        ln=ln.split('\t')
                        total_search_time = total_search_time + float(ln[1])
            # if testId=='sald1m1':
            #     print('Sald issue deb', sorted(unique_nodes))
            dct=None
            if not (testId in all_results):
                all_results[testId]={}
            dct=all_results[testId]
            dct['Total Match']=total_count
            dct['Recall']=total_count/(q*k)
            dct['Search Time']=total_search_time
            dct['Given n*k']=q*k
            dct['Found n*k']=total_nb
            dct['unique neighbors']=len(unique_nodes)
            dct['issue']=(total_nb!=q*k)
    except:
        rr=3
for fileName in os.listdir():
    if ('build' in fileName):
        testId=fileName.split('.txt')[0].split('build')[1]
        with open(fileName, 'r') as file:
            Lines = file.readlines()
            for ln in Lines:
                if 'Index_building_total_time_secs' in ln:
                    ln=ln.replace('\n','')
                    ln=ln.split('\t')
                    total_indexing_time = float(ln[1])
                    dct=None
                    if not (testId in all_results):
                        all_results[testId]={}
                    dct=all_results[testId]
                    dct['Total Indexing Count']=total_indexing_time
                
        
        
# print(all_results)
org_results=[]
for key in all_results.keys():
    org_results.append((key, all_results[key]))
for r in sorted(org_results):
    print(r)
