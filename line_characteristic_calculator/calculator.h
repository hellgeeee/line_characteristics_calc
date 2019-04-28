#pragma once
#include <collection.h>
#include <utility> //for std::move
#include <vector>

#define _FAKE_VALUE 512000.0

using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;

namespace logical {
	using namespace std;
	using namespace Windows::Foundation;

	public ref class Calculator sealed
	{
	public:
		Calculator();
		float calculateStep();
		float calculateLinesAngle();
		IVector<float>^ calculateLinesCross(); //x, y
		void calculateApproximatedCurve(); // x, y of center, decline in degrees, big ax/2, small ax/2 // Метод перебора
		void calculateApproximatedCurve_2(); // x, y of center, decline in degrees, big ax/2, small ax/2 // Кусочно-полиномиальная аппроксимация

		void clear();

		// get and set functions

		IVector<float>^ get_rez_curve();
		int get_input_size();
		float get_x_by_index(int index);
		float get_y_by_index(int index);
		void add_point(Point p);

		float get_a_begin_x();
		float get_a_begin_y();
		void set_a_begin(int index);

		float get_a_end_x();
		float get_a_end_y();
		void set_a_end(int index);

		float get_b_begin_x();
		float get_b_begin_y();
		void set_b_begin(int index);

		float get_b_end_x();
		float get_b_end_y();
		void set_b_end(int index);

		float get_c_begin_x();
		float get_c_begin_y();
		void set_c_begin(int index);

		float get_c_end_x();
		float get_c_end_y();
		void set_c_end(int index);

		float get_d_begin_x();
		float get_d_begin_y();
		void set_d_begin(int index);

		float get_d_end_x();
		float get_d_end_y();
		void set_d_end(int index);

		float get_e_begin_x();
		float get_e_begin_y();

		float get_e_end_x();
		float get_e_end_y();
		void set_e(int begin_index, int end_index);

	private:
		struct _Point {
		public:
			float x;
			float y;
			float z;
		};

		vector<_Point> a;
		vector<_Point> b;
		vector<_Point> c;
		vector<_Point> d;
		vector<_Point> e;
		vector<_Point> all_points;
		IVector<float>^ rez_curve;

		_Point get_point_by_index(int index);
		void readFile();
	};

}
