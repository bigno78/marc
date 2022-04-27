# marc

`marc` is a tool for visualizing sparse matrices in matrix market format.

## Could you tell us a bit more?

Basically, it produces an image that displays the density of non-zeroes in the sparse matrix. Each part of the matrix is colored and the higher the intensity of the color the more non-zeros in that particular region of the matrix. Here is one example:

TODO: add image

To do this, the matrix is split into square blocks and the number of non-zeros in each block is calculated. Then, based on the number of non-zeros compared to the capacity of the block an appropriate color for the output can be chosen. 

The size of the blocks depends on the size of the matrix. Obviously, the more blocks the better quality of the output.

This can help you to identify the underlying structure of the matrix and see any symmetries or regularities. Since for my diploma thesis I work on implementing sparse matrix-vector multiplication on the gpu

Using this information you can then for instance choose an appropriate sparse matrix format 


