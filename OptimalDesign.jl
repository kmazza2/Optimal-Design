module OptimalDesign

export orthonormal_nullspace_basis
export orthonormal_columnspace_basis
export orthogonal_subspace_basis
export sample_L_ensemble
export weighted_sample
export bernoulli_trial
export Eigenpair
export SpectralDecomposition
export orthonormal_spectral_decomposition
export orthonormal_spectral_decomposition_matrix

import LinearAlgebra as LA

"Returns orthonormal basis for nullspace of A as columns of returned matrix."
function orthonormal_nullspace_basis(A, tol)
    # use SVD to find orthnormal basis for nullspace of A
    #
    # columns of V corresponding to zeros on diagonal of S
    # form orthonormal basis for nullspace of matrix
    # (Trefethen p. 33, p. 36)
    decomp = LA.svd(A, full=true)
    nullspace_basis = decomp.V[
        :,
        [
            abs.(decomp.S);
            zeros(size(A,2) - length(decomp.S))
        ] .< tol
    ]
    return nullspace_basis
end


"Returns orthonormal basis for columnspace of A as columns of returned matrix."
function orthonormal_columnspace_basis(A, tol)
    # use SVD to find orthnormal basis for columnspace of A
    #
    # columns of U corresponding to nonzero values on diagonal
    # of S form orthonormal basis for column space of matrix
    # (Trefethen p. 33, p. 36)
    decomp = LA.svd(A, full=true)
    columnspace_basis = decomp.U[
        :,
        [
            abs.(decomp.S);
            zeros(size(A,1) - length(decomp.S))
        ] .> tol
    ]
    return columnspace_basis
end

"Returns orthonormal basis for subspace of column space of A orthogonal to u."
function orthogonal_subspace_basis(A, u, tol)
    nullspace_basis = orthonormal_nullspace_basis(u' * A, tol)
    # columns of A * nullspace_basis are spanning set for subspace of
    # column space of A orthogonal to u
    #
    # find orthonormal basis for column space of A * nullspace_basis
    # (since the column space is the subspace of the column space
    # of A orthogonal to u, this completes the computation)
    spanning_set_basis = orthonormal_columnspace_basis(
        A * nullspace_basis, tol
    )
    return spanning_set_basis
end

function sample_L_ensemble(L, tol, rng)
    @assert LA.issymmetric(L)
    v, M = LA.eigen(L)
    @assert all(v .>= 0.0)
    N = size(L)[1]
    Y = Set()
    V = M[:, map(l -> bernoulli_trial(l / (l + 1.0), rng), v)]
    while size(V)[2] != 0
        weights = LA.diag(V * V')
        i = weighted_sample(weights, rng)
        Y = union(Y, Set(i))
        V = orthogonal_subspace_basis(V, LA.I[1:N,i], tol)
    end
    return Y
end

function weighted_sample(weights, rng)
    N = length(weights)
    weights = weights ./ sum(weights)
    return min(N - sum(rand(rng) .< cumsum(weights)) + 1, N)
end

function bernoulli_trial(p, rng)
    return rand(rng) < p
end

struct Eigenpair
    val::Float64
    vec::Vector{Float64}
end

struct OrthonormalSpectralDecomposition
    eigenpairs::Vector{Eigenpair}
    original_matrix_size::Tuple{Int, Int}
    function OrthonormalSpectralDecomposition(
            eigenpairs::Vector{Eigenpair},
            original_matrix_size::Tuple{Int, Int},
            tol
    )
        if original_matrix_size[1] <= 0 || original_matrix_size[2] <= 0
            error(
                "original matrix must have positive numbers of rows and columns"
            )
        end
        if original_matrix_size[1] != original_matrix_size[2]
            error(
                "original matrix must be square"
            )
        end
        n = length(eigenpairs)
        for i in 1:n
            if abs(LA.norm(eigenpairs[i].vec) - 1) > tol
                error("eigenvector not unit length")
            end
        end
        for i in 1:(n-1)
            for j in (i+1):n
                if (
                        abs(eigenpairs[i].val) > tol &&
                        abs(eigenpairs[j].val) > tol &&
                        abs(eigenpairs[i].vec' * eigenpairs[j].vec) > tol
                )
                    error("eigenvectors not orthogonal")
                end
            end
        end
        return new(eigenpairs, original_matrix_size)
    end
end

function orthonormal_spectral_decomposition(A::Matrix{Float64}, tol)
    if !LA.issymmetric(A)
        throw(DomainError("A must be symmetric"))
    end
    eigenvalues = LA.eigvals(A)
    eigenpairs = Vector{Eigenpair}()
    for eigenvalue in eigenvalues
        if abs(eigenvalue) > tol
            ortho_nullspace_basis = orthonormal_nullspace_basis(
                A - eigenvalue * LA.I,
                tol
            )
            nullspace_dim = size(ortho_nullspace_basis, 2)
            for i in 1:nullspace_dim
                push!(
                    eigenpairs,
                    Eigenpair(eigenvalue, ortho_nullspace_basis[:, i])
                )
            end
        end
    end
    return OrthonormalSpectralDecomposition(eigenpairs, size(A), tol)
end

function orthonormal_spectral_decomposition_matrix(decomp::OrthonormalSpectralDecomposition)
    if length(decomp.eigenpairs) == 0
        return zeros(
            decomp.original_matrix_size[1],
            decomp.original_matrix_size[2]
        )
    else
        return sum(
            [pair.val * pair.vec * pair.vec' for pair in decomp.eigenpairs]
        )
    end
end

end
