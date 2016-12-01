#include "Matrix.hpp"

#include <cmath>

#include <stdexcept>

// standard matrix multiplication
Matrix Matrix::operator* (const Matrix &B) const {
	const Matrix &A = *this;
	ASSERT_DOMAIN(A.Width() == B.Height());

	Matrix C(B.Width(), A.Height(), 0);
	const size_t depth = A.Width();

	for(size_t y = 0; y < A.Height(); ++y)
		for(size_t x = 0; x < B.Width(); ++x)
			for(size_t i = 0; i < depth; ++i)
				C[y][x] += A[y][i] * B[i][x];

	return C;
}

// hadamard product
Matrix Matrix::operator% (const Matrix &B) const {
	const Matrix &A = *this;
	ASSERT_DOMAIN(A.Width() == B.Width() && A.Height() == B.Height());

	Matrix C(Width(), Height(), 0);
	for(size_t i = 0; i < Height(); ++i)
		for(size_t j = 0; j < C.Width(); ++j)
			C[i][j] = A[i][j] * B[i][j];
	return A;
}

Matrix Matrix::Transpose() const {
	Matrix result(Height(), Width());

	for(size_t x = 0; x < Width(); ++x) {
		for(size_t y = 0; y < Height(); ++y)
			result[x][y] = grid_[y][x];
	}

	return result;
}

Matrix Matrix::Invert() const {
	ASSERT_DOMAIN(Square() && Det() != scalar_t(0));
	const size_t dim = Height();
	return GaussianElimination(this->ConcatenateColumns(Matrix(dim))).SubMatrix(dim, dim);
}

Matrix Matrix::RowEchelon() const {
	Matrix result(*this);
	size_t row = 0, col = 0;
	while(col != Width() || row != Height()) {
		const scalar_t leading_entry = result[row][col];
		if(leading_entry == 0.) {
			size_t new_row = row;
			while(++new_row < Height()) {
				if(result[new_row][col] == 0.)
					continue;
				result.SwapRows(row, new_row);
				break;
			}
			++col;
			continue;
		}
		for(size_t i = row + 1; i < Height(); ++i) {
			const scalar_t reduced_entry = result[i][col];
			result = result.AddToRow(row, i, -reduced_entry / leading_entry);
		}
		++row;
	}
	return result;
}

Matrix Matrix::RowEchelonReduced() const {
	Matrix result(*this);
	size_t row = 0, col = 0;
	while(col != Width() || row != Height()) {
		const scalar_t leading_entry = result[row][col];
		if(leading_entry == 0.) {
			size_t new_row;
			while(++new_row < Height()) {
				if(result[new_row][col] == 0.)
					continue;
				result.SwapRows(row, new_row);
				break;
			}
			++col;
			continue;
		}
		result = result.MultiplyRow(row, 1. / leading_entry);
		for(size_t i = 0; i < Height(); ++i) {
			if(i == row)
				continue;
			const scalar_t reduced_entry = result[i][col];
			result = result.AddToRow(row, i, -reduced_entry);
		}
		++row;
	}
	return result;
}

Matrix Matrix::ColumnEchelon() const {
	return this->Transpose().RowEchelon().Transpose();
}

Matrix Matrix::ColumnEchelonReduced() const {
	return this->Transpose().RowEchelonReduced().Transpose();
}

Matrix Matrix::GaussianElimination(const Matrix &M) {
	// TODO: prevent segfault in some cases
	Matrix result = M;

	bool *free = new bool[M.Height()];
	for(size_t i = 0; i < M.Height(); ++i)
		free[i] = true;

	for(size_t clm = 0; clm < M.Width(); ++clm) {
		for(size_t ln = 0; ln < M.Height(); ++ln)
			if(result[ln][clm] && free[ln]) {
				free[ln] = false;
				result = result.MultiplyRow(ln, 1. / result[ln][clm]);
				for(size_t ln2 = 0; ln2 < M.Height(); ++ln2)
					if(result[ln2][clm] && ln != ln2) {
						result = result.AddToRow(ln, ln2, -result[ln2][clm] / result[ln][clm]);
					}
				break;
			}
	}

	delete [] free;
	return result;
}

std::pair <Matrix, Matrix> Matrix::LUDecomposition(const Matrix &M) {
	ASSERT_DOMAIN(M.Square());

	const size_t dim = M.Height();
	Matrix
		L = M.LowerTriangular(),
		U = M.UpperTriangular();

	for(size_t i = 0; i < dim; ++i)
		U[i][i] = 1;

	for(size_t i = 0; i < dim; ++i) {
		for(size_t j = i; j < dim; ++j)
			for(size_t k = 0; k < i; ++k)
				L[j][i] -= L[j][k] * U[k][i];

		for(size_t j = i; j < dim; ++j) {
			scalar_t sum = 0;
			for(size_t k = 0; k < i; ++k)
				sum += L[i][k] * U[k][j];

			if(L[i][i] == scalar_t(0))
				throw scalar_t(0);
			U[i][j] = (M[i][j] - sum) / L[i][i];
		}
	}

	return std::pair <Matrix, Matrix> (L, U);
}
