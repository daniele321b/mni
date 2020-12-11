import numpy as np

import scipy.linalg as sla

# ## Problema
# 4 utenti e loro valutazioni per 6 stagioni
# 
# |        | Ryne | Erin | Nathan | Pete |
# | --- | --- | --- | --- | --- |
# | season 1 | 5 | 5 | 0 | 5 |
# | season 2 | 5 | 0 | 3 | 4 |
# | season 3 | 3 | 4 | 0 | 3 |
# | season 4 | 0 | 0 | 5 | 3 |
# | season 5 | 5 | 4 | 4 | 5 |
# | season 6 | 5 | 4 | 5 | 5 |



seasons = [1,2,3,4,5,6]
users = ['Ryne', 'Erin', 'Nathan', 'Pete']

A = np.array([            
    [5,5,0,5], # season 1
    [5,0,3,4], # season 2
    [3,4,0,3], # season 3
    [0,0,5,3], # season 4
    [5,4,4,5], # season 5
    [5,4,5,5]  # season 6
    ], dtype=float)


# ## Calcoliamo la SVD di A

print('dimensions of U, s and V')
U, S, V = sla.svd(A)
print(U.shape)
print(S.shape)
print(V.shape)
print('vector of singular values=',S)


# ### rappresentiamo i dati in uno spazio a due dimensioni

U3 = U[:,:3]
V3 = V.T[:,:3]
S3 = np.diag(S[:3])
print('SVD troncata con k=3. Stampa di U, s e V con 2 cifre decimali')
print('U3=',U3.round(2))
print('S3=',S3.round(2))
print('V3=',V3.round(2))






# ## Obiettivo: trovare utenti simili per fornire raccomandazioni
# 
# Aggiungiamo l'utente Luke, che dà le valutazioni seguenti per le stagioni:
# 5,5,0,0,0,5.
# Per fornire raccomandazioni a Luke troviamo gli utenti più simili a Luke,
# rappresentando Luke nello spazio 2D in cui abbiamo rappresentato gli utenti

luke = np.array([5,5,0,0,0,5])
print('valutazioni delle stagioni di Luke',luke)


# 
# Per proiettare le valutazioni di Luke nello spazio 2D degli utenti, se esse sono indicate con L
# 
# L^T * U_2 * S_2^{-1}
# 

luke3d = luke.dot(U3.dot(np.linalg.inv(S3)))
print('valutazioni di Luke proiettate nello spazio 3D')
print(luke3d)





print(luke3d.shape)

for i,xy in enumerate(V3):
    angle = np.dot(xy, luke3d) / (np.linalg.norm(xy) * np.linalg.norm(luke3d))
    print("coseno dell'angolo tra %s e %s: %2.2g" % ('luke', users[i], angle))

    

