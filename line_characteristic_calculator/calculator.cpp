#include <string>
#include "pch.h"
#include "calculator.h"


using namespace logical;
using namespace std;

Calculator::Calculator(void)
{
	readFile();
	_Point p_tmp;
	p_tmp.x = _FAKE_VALUE;
	a.push_back(p_tmp);
	a.push_back(p_tmp);
	b.push_back(p_tmp);
	b.push_back(p_tmp);
	c.push_back(p_tmp);
	c.push_back(p_tmp);
	d.push_back(p_tmp);
	d.push_back(p_tmp);
	e.push_back(p_tmp);
	e.push_back(p_tmp);
}

// get-set functions
void Calculator::clear(void) {
	all_points.clear();
}

Calculator::_Point Calculator::get_point_by_index(int index) {

	if (all_points.size() > index) {
		return all_points.at(index);
	}
	else {
		_Point p;
		p.x = 0;
		p.y = 0;
		return p;// 
	}
}

float Calculator::get_x_by_index(int index) {
	if (all_points.size() > index) {
		return all_points.at(index).x;
	}
	else {
		return 0;// 
	}
}

float Calculator::get_y_by_index(int index) {
	if (all_points.size() > index) {
		return all_points.at(index).y;
	}
	else {
		return 0;// 
	}
}

void Calculator::add_point(Point p) {
	_Point p_tmp;
	p_tmp.x = p.X;
	p_tmp.y = p.Y;
	all_points.push_back(p_tmp);
}

void Calculator::readFile() {
	fstream *file_stream;

	for (int i = 0; i < 100; i++) {
		_Point p;
		p.x = rand() % 300; // ориентируясь на размер экрана 
		p.y = rand() % 300;
		all_points.push_back(p);
	}
}

// calculus functions
float Calculator::calculateStep() {
	if ((b.front().x != _FAKE_VALUE) && (a.front().x != _FAKE_VALUE)) {
		return b.front().y - a.back().y;
	}
	else {
		return _FAKE_VALUE;
	}
}

float Calculator::calculateLinesAngle() {
	if ((c.front().x != _FAKE_VALUE) && (d.front().x != _FAKE_VALUE)) {

		float dot_production = (c.back().x - c.front().x)*(d.back().x - d.front().x) + (c.back().y - c.front().y)*(d.back().y - d.front().y);
		// тогда нет смысла далее
		if (dot_production == 0) {
			return 90;
		}

		float vertice_c_length = sqrt(pow((c.back().x - c.front().x), 2) + pow((c.back().y - c.front().y), 2));
		float vertice_d_length = sqrt(pow((d.back().x - d.front().x), 2) + pow((d.back().y - d.front().y), 2));

		// будем считать, 512 - код невозможности вычисления
		if ((vertice_c_length == 0) && (vertice_d_length == 0)) {
			return 512;
		}

		// арккосинус от скалярного произведения векторов, деленного на их длины
		float ans = acos(dot_production / (vertice_c_length*vertice_d_length));

		// перевод в градусы
		ans = ans * 180 / 3.14;

		// расчет против часовой стрелки от прямой c к прямой d
		// получается неточно. например, при угле 120 - на 6 градусов, думаю, погрешность представления.
		return ans;

	}
	else {
		return _FAKE_VALUE;
	}
}

IVector<float>^ Calculator::calculateLinesCross(void) {
	IVector<float>^ v_tmp = ref new Vector<float>();

	if ((c.front().x != _FAKE_VALUE) && (d.front().x != _FAKE_VALUE)) {

		// представляем обе прямые как k*x + l. найдя коэффициенты, найдем и точку пересечения.
		float k_c = (c.back().y - c.front().y) / (c.back().x - c.front().x);
		float l_c = c.front().y - c.front().x / (c.back().x - c.front().x) * (c.back().y - c.front().y);

		float k_d = (d.back().y - d.front().y) / (d.back().x - d.front().x);
		float l_d = d.front().y - d.front().x / (d.back().x - d.front().x) * (d.back().y - d.front().y);
		_Point ans;
		ans.x = (l_d - l_c) / (k_c - k_d);
		ans.y = k_c * ans.x + l_c;

		v_tmp->Append(ans.x);
		v_tmp->Append(ans.y);
		return v_tmp;

	}
	else {
		v_tmp->Append(_FAKE_VALUE);
		return v_tmp;
	}
}

//Windows::Foundation::IAsyncAction^ 
void Calculator::calculateApproximatedCurve(void) {
	//	return concurrency::create_async([this](Windows::Storage::Streams::IRandomAccessStreamWithContentType^ stream)
	//	{

	IVector<float>^ rez = ref new Vector<float>();

	if (e.front().x != _FAKE_VALUE) {

		float center_x, center_y, cemiax_big, cemiax_small, decline, excentricity;
		// нечто вроде МНК. Минус: медленно работает.
		float deflection = 0;
		float min_deflection = 200;

		float step = 1, ang_step = .15;
		int nodes_number = 0;
		float max_semiaxis_size = 20;


		float ax1_rez, ax2_rez;
		for (float ax1 = step; ax1 < max_semiaxis_size; ax1 += step) {
			for (float ax2 = step; ax2 < max_semiaxis_size; ax2 += step) {

				for (float x0 = 0; x0 < max_semiaxis_size; x0 += step) {
					for (float y0 = 0; y0 < max_semiaxis_size; y0 += step) {

						for (float dec = 0; dec < 3.14; dec += ang_step) {

							deflection = 0;
							// эллипс: (x/ax1)^2 + (y/ax2)^2 = 1;
							// перебираем центральные точки, полуоси и наклоны (с учетом предполагаемых нами диапазонов). Подставляем в такое уравнение имеющиеся точки
							// при этом пользуемся тем, что: (x/ax1)^2 + (y/ax2)^2 < (x/ax1 + y/ax2)^2 - т.к. понимаем, что возведение в квадрат - дорогостоящая операция
							for (int i = 0; i < e.size(); i++) { // проходимся по всем точкам предполагаемого эллипса
								deflection += abs(
									pow((e.at(i).x - x0) / ax1, 2) +
									pow(abs(e.at(i).y - y0) / ax2, 2) - 1
								);
							}
							if (deflection < min_deflection) {
								min_deflection = deflection;
								center_x = x0;
								center_y = y0;
								ax1_rez = ax1;
								ax2_rez = ax2;
								if (ax1 < ax2) {
									cemiax_small = ax1;
									cemiax_big = ax2;
								}
								else {
									cemiax_small = ax2;
									cemiax_big = ax1;
								}
								decline = dec;
							}

						}

					}
				}


			}
		}

		excentricity = sqrt(1 - pow(cemiax_small / cemiax_big, 2));

		rez->Append(center_x);
		rez->Append(center_y);
		rez->Append(decline);
		rez->Append(ax1_rez);
		rez->Append(ax2_rez);
		rez->Append(excentricity);

	}
	else {
		rez->Append(_FAKE_VALUE);
	}

	rez_curve = rez;

	//	}); // async method
}

void Calculator::calculateApproximatedCurve_2() { // Кусочно-полиномиальная аппроксимация
	// метод годится только для трех точек и более
	if (e.size() < 3) {
		rez_curve->Append(_FAKE_VALUE);
		return;
	}

	// уравнение b^2*(x-x0)^2 + a^2*(y-y0)^2 = b^2*a^2 - описывает наш эллипс
	// но он может быть и с уклоном, т.е. в общем случае запишется a0*x^2 + a1*x*y + a2*y^2 = a3
	// если такую форму мы поделим на a3, то получим в такой форме: a0*x^2 + a1*x*y + a2*y^2 = 1
	// В матричной записи будет так X*a=1 (единичный столбец)
	// Из нее найдем a: a = X^-1 * 1 (т. е. a[0] = сумма всех элементов первой строки X^-1, a[1] = сумма всех элементов первой строки, a[2] = сумма всех элементов третьей строки)

	float a[3] = { 0, 0, 0 };
	float a_avg[3] = { 0, 0, 0 }; //здесь храним среднее арифметическое всех коэффициентов a

	// определяем матрицу всех точек
	int n = e.size();
	vector<vector<float>> X;
	for (int i = 0; i < n; i++) {
		X.push_back(vector<float>());
		X[i].push_back(pow(e[i].x, 2));
		X[i].push_back(e[i].x * e[i].y);
		X[i].push_back(pow(e[i].y, 2));
	}

	for (int begin = 0; begin <= n - 3; begin++)
	{	// для одного трехточечного блока
		// нас интересуют только трехмерные матрицы, поэтому копируем часть из всего пула
		float X_part[3][3];
		for (int i = begin; i < begin + 3; i++)
			for (int j = 0; j < 3; j++)
				X_part[i][j] = X[i][j];

		// находим обратную матрицу, для этого:
		// 1. находим матрицу миноров (и за одно определитель)
		float X_minors[3][3];
		float det = 0;
		for (int i = 0; i < begin + 3; i++)
			for (int j = 0; j < 3; j++)
				for (int k1 = 0; k1 < 3; k1++)
					for (int k2 = 0; k2 < 3; k2++)
						if (k1 != i)
							if ((k2 != j) && (k2 != k1)) {
								X_minors[i][j] = pow(-1, i + j) * (X_part[k1][k1] * X_part[k2][k2] - X_part[k1][k2] * X_part[k2][k1]);
								det += X_part[i][j] * X_minors[i][j];
							}

		// 2. транспонируем
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++) {
				float tmp = X_minors[i][j];
				X_minors[i][j] = X_minors[j][i];
				X_minors[j][i] = tmp;
			}

		// 3. делим на определитель
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				X_minors[i][j] /= det;

		// полученную матрицу умножаем на единичный вектор
		for (int i = 0; i < 3; i++) {
			for (int j = 0; i < 3; i++)
				a[i] += X_minors[i][j];

			// приращиваем среднее значение
			if (a_avg[i] > 0)
				a_avg[i] = (a_avg[i] + a[i]) / 2;
			else
				a_avg[i] = a[i];
		}


	}

	IVector<float>^ rez = ref new Vector<float>();
	rez->Append(a_avg[0]);
	rez->Append(a_avg[1]);
	rez->Append(a_avg[2]);
	rez_curve = rez;
}

// get and set methods
IVector<float>^  Calculator::get_rez_curve() {
	return rez_curve;
}
int Calculator::get_input_size() {
	return all_points.size();
}
float Calculator::get_a_begin_x() {
	return a.front().x;
}
float Calculator::get_a_begin_y() {
	return a.front().y;
}
void Calculator::set_a_begin(int index) {
	a.front() = get_point_by_index(index);
}

float Calculator::get_a_end_x() {
	return a.back().x;
}
float Calculator::get_a_end_y() {
	return a.back().y;
}
void Calculator::set_a_end(int index) {
	a.back() = get_point_by_index(index);
}

float Calculator::get_b_begin_x() {
	return b.front().x;
}
float Calculator::get_b_begin_y() {
	return b.front().y;
}
void Calculator::set_b_begin(int index) {
	b.front() = get_point_by_index(index);
}

float Calculator::get_b_end_x() {
	return b.back().x;
}
float Calculator::get_b_end_y() {
	return b.back().y;
}
void Calculator::set_b_end(int index) {
	b.back() = get_point_by_index(index);
}

float Calculator::get_c_begin_x() {
	return c.front().x;
}
float Calculator::get_c_begin_y() {
	return c.front().y;
}
void Calculator::set_c_begin(int index) {
	c.front() = get_point_by_index(index);
}

float Calculator::get_c_end_x() {
	return c.back().x;
}
float Calculator::get_c_end_y() {
	return c.back().y;
}
void Calculator::set_c_end(int index) {
	c.back() = get_point_by_index(index);
}

float Calculator::get_d_begin_x() {
	return d.front().x;
}
float Calculator::get_d_begin_y() {
	return d.front().y;
}
void Calculator::set_d_begin(int index) {
	d.front() = get_point_by_index(index);
}

float Calculator::get_d_end_x() {
	return d.back().x;
}
float Calculator::get_d_end_y() {
	return d.back().y;
}
void Calculator::set_d_end(int index) {
	d.back() = get_point_by_index(index);
}

float Calculator::get_e_begin_x() {
	return e.front().x;
}
float Calculator::get_e_begin_y() {
	return e.front().y;
}

float Calculator::get_e_end_x() {
	return e.back().x;
}
float Calculator::get_e_end_y() {
	return e.back().y;
}
void Calculator::set_e(int begin_index, int end_index) {
	e.clear();
	for (int i = begin_index; i <= end_index; i++) {
		e.push_back(get_point_by_index(i));
	}
}