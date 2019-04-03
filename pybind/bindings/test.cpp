#include <memory>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

namespace py = pybind11;

class Matrix
{
public:
    Matrix(std::size_t rows, std::size_t cols)
        : m_rows{rows}
        , m_cols{cols}
    {
        m_data = new float[rows*cols];
    }
    float *data() { return m_data; }
    std::size_t rows() const { return m_rows; }
    std::size_t cols() const { return m_cols; }
private:
    std::size_t m_rows;
    std::size_t m_cols;
    float *m_data;
};

PYBIND11_MODULE(example, m)
{
py::class_<Matrix>(m, "Matrix", py::buffer_protocol())
    .def(py::init<std::size_t, std::size_t>())
    .def_buffer([](Matrix &m) -> py::buffer_info {
        return py::buffer_info(
            m.data(),                               /* Pointer to buffer */
            sizeof(float),                          /* Size of one scalar */
            py::format_descriptor<float>::format(), /* Python struct-style format descriptor */
            2,                                      /* Number of dimensions */
            { m.rows(), m.cols() },                 /* Buffer dimensions */
            { sizeof(float) * m.rows(),             /* Strides (in bytes) for each index */
              sizeof(float) }
        );
    });
}
