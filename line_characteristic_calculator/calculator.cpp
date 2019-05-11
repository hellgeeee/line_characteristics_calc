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
void Calculator::calculateApproximatedCurve_bruteForceMethod(void) {
	//	return concurrency::create_async([this](Windows::Storage::Streams::IRandomAccessStreamWithContentType^ stream)
	//	{

	IVector<float>^ rez = ref new Vector<float>();

	if (e.front().x != _FAKE_VALUE) {

		float x_center = _FAKE_VALUE, y_center = _FAKE_VALUE, cemiax_big = _FAKE_VALUE, cemiax_small = _FAKE_VALUE, decline_ang = _FAKE_VALUE, excentricity = _FAKE_VALUE;
		// нечто вроде МНК. Минус: медленно работает.
		float deflection = 0;
		float min_deflection = 200;

		float step = 1, ang_step = .15;
		int nodes_number = 0;
		float max_semiaxis_size = 20;


		float cemiax1, cemiax2;
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
									pow(  ( (e.at(i).x - x0)*cos(dec) + (e.at(i).y - y0)*sin(dec) )   / ax1, 2) +
									pow(  ( (e.at(i).x - x0)*(-sin(dec)) + (e.at(i).y - y0)*cos(dec) ) / ax2, 2) - 1
								);
							}
							if (deflection < min_deflection) {
								min_deflection = deflection;
								x_center = x0;
								y_center = y0;
								cemiax1 = ax1;
								cemiax2 = ax2;
								if (ax1 < ax2) {
									cemiax_small = ax1;
									cemiax_big = ax2;
								}
								else {
									cemiax_small = ax2;
									cemiax_big = ax1;
								}
								decline_ang = dec;
							}

						}

					}
				}


			}
		}

		excentricity = sqrt(1 - pow(cemiax_small / cemiax_big, 2));

		rez->Append(x_center);
		rez->Append(y_center);
		rez->Append(cemiax1);
		rez->Append(cemiax2);
		rez->Append(decline_ang);
		rez->Append(excentricity);
	}
	else {
		rez->Append(_FAKE_VALUE);
	}

	rez_curve = rez;

	//	}); // async method
}

void Calculator::calculateApproximatedCurve_partialPolinomialMethod() { // Кусочно-полиномиальная аппроксимация
	// метод годится только для 5 точек и более
	// проблема метода: работает только если точки точно принадлежат эллипсу. Иначе выдаст уравнение кривой, которая эллипсом не является.
	int n = 5;
	if (e.size() < n) {
		rez_curve = ref new Vector<float>();
		rez_curve->Append(_FAKE_VALUE);
		return;
	}

	// уравнение b^2*(x-x0)^2 + a^2*(y-y0)^2 = b^2*a^2 - описывает наш эллипс - это канонический вид
	// но в общем виде запишется так A*x^2 + 2*В*x*y + С*y^2 + D*x + E*y + F = 0
	// 1. найдем А, B, C, D, E, F, подставив все точки (их должно быть минимум 6, чтобы 6 уравнений и 6 неизвестных). Значит мы нашли уравнение общего вида
	// есть 3 инварпанта У этого уравнения: определитель, составленный из всех коэффициентов; определитель, сост. из А, B, C; разность D-E.
	// 2. т.е. они одинаковы и для уравнения общего вида, и для канонического. Найдя их, найдем уравнение канонического вида
	// 3. по каноническому виду найдем полуоси, центр и угол поворота
	float A = 0, B = 0, C = 0, D = 0, E = 0, F = 0;
	float A_avg = 0, B_avg = 0, C_avg = 0, D_avg = 0, E_avg = 0, F_avg = 0; // здесь храним среднее арифметическое всех коэффициентов

	// 1. 
	// определяем матрицу X всех точек, где в строке будет x^2, y^2, xy, x, y. Тогда X*(A B 2C 2D 2E)=1
	// = 1, потому что предполагаем, все уравнение поделено на свободный член
	// решать уравнение относительно A B 2C 2D 2E будем методом Крамера
	int points_num = e.size();
	vector<vector<float>> X;
	for (int i = 0; i < points_num; i++) {
		X.push_back(vector<float>());
		X[i].push_back(pow(e[i].x, 2));
		X[i].push_back(e[i].x * e[i].y);
		X[i].push_back(pow(e[i].y,2));
		X[i].push_back(e[i].x);
		X[i].push_back(e[i].y);
	}

	// инициализируем матрицу 5x5
	vector<vector<float>> mat_tmp;
	for (int i = 0; i < n; i++) {
		vector<float> vec_tmp;
		for (int i = 0; i < n; i++)	vec_tmp.push_back(0);
		mat_tmp.push_back(vec_tmp);
	}

	int begin;
	for (begin = 0; begin <= points_num - n; begin++)
	{	// для одного 5x5-точечного блока копируем часть из всего пула
		for (int i = begin; i < begin + n; i++) {
			vector<float> vec_tmp;
			for (int j = 0; j < n; j++)
				mat_tmp[i - begin][j] = X[i][j];
		}

		// находим определитель блока
		float det = determinant(mat_tmp);
		if (det == 0) {
			// some extra actioins
		}

		// для проверки
		/*		vector<vector<float>> mat_delete;
				vector<float> vec_delete;  vec_delete.push_back(2); vec_delete.push_back(3);
				mat_delete.push_back(vec_delete);
				mat_delete.push_back(vec_delete);
				mat_delete[1][1] = 10; mat_delete[1][1] = 5;
				float det_delete = determinor(mat_delete);
		*/
		float missed_koefs[5];
		for (int i = 0; i < n; i++) {
			missed_koefs[i] = determinant(replaceByUnicolumt(mat_tmp, i)) / det;
		}

		A = missed_koefs[0];
		B = missed_koefs[1] / 2;
		C = missed_koefs[2];
		D = missed_koefs[3] / 2;
		E = missed_koefs[4] / 2;
		F = -1;

		A_avg += A;
		B_avg += B;
		C_avg += C;
		D_avg += D;
		E_avg += E;

	}
	A_avg /= begin + 1;
	B_avg /= begin + 1;
	C_avg /= begin + 1;
	D_avg /= begin + 1;
	E_avg /= begin + 1;
	F_avg = -1;

	// далее приводим уравнение к каноническому виду - методом инвариантов
	vector<vector<float>> inv_mat_tmp;
	vector<float> vec_tmp;
	vec_tmp.push_back(A_avg);
	vec_tmp.push_back(B_avg);
	vec_tmp.push_back(D_avg);
	inv_mat_tmp.push_back(vec_tmp);
	vec_tmp[0] = B_avg;
	vec_tmp[1] = C_avg;
	vec_tmp[2] = E_avg;
	inv_mat_tmp.push_back(vec_tmp);
	vec_tmp[0] = D_avg;
	vec_tmp[1] = E_avg;
	vec_tmp[2] = F_avg;
	inv_mat_tmp.push_back(vec_tmp);

	float inv_1 = A_avg + C_avg;
	float inv_2 = A_avg*C_avg - B_avg*B_avg;
	float inv_3	= determinant(inv_mat_tmp);

	// Найдем коэф-ты канонического уравнения из системы
	// A_canonical + C_canonical = inv1; A_canonical * C_canonical = inv2; A_canonical * C_canonical * F_canonical = inv_3
	// отсюда 
	float F_canonical = - inv_3 / inv_2;  
	float C_canonical = (inv_1 + sqrt(pow(inv_1, 2) - 4 * inv_2)) / 2;
	float A_canonical = inv_1 - C_canonical;

	// Вытащим характеристики найденного эллипса
	float x_center = (C_avg * D_avg - E_avg)/(B_avg - A_avg * C_avg);
	float y_center = -D_avg - A_avg * x_center;

	float decline_ang;
	if (B_avg != 0)
		decline_ang = atan((A_canonical - A_avg)/B_avg) / 3.14 * 180;
	else
		decline_ang = 0;
	float cemiax1 = sqrt(abs(1 / A_canonical / -F_canonical));
	float cemiax2 = sqrt(abs(1 / C_canonical / -F_canonical));

	// далее вытаскиваем характеристики эллипса из уравнения канонического вида

	IVector<float>^ rez = ref new Vector<float>();
	rez->Append(x_center);
	rez->Append(y_center);
	rez->Append(cemiax1);
	rez->Append(cemiax2);
	rez->Append(decline_ang);
	rez_curve = rez;
}

void Calculator::calculateApproximatedCurve_bruteForceMethod_optimized(void) {
	// метод - нечто вроде МНК. Минус: медленно работает.

	// 0. обработали ситуацию, когда точек недостаточно
	IVector<float>^ rez = ref new Vector<float>();
	if (e.front().x == _FAKE_VALUE) {
		rez->Append(_FAKE_VALUE);
		rez_curve = rez;
		return;
	}

	// считаем, что работаем с функциональной зависимостью, т.е. x[i] <= x[i+1] для любого i
	// здесь пытаемся разобраться, в каких пределах можно искать эллипс

	// 1. убывает кривая или возрастает?
	bool is_curve_increase = (e[0].y < e[1].y)||(e[1].y < e[2].y);

	// 2. В каких пределах искать полуоси эллипса?
	float ax_x_min = (e.back().x - e.front().x)/2;
	float ax_x_max = (e.back().x - e.front().x)*2;
	float ax_y_min = _FAKE_VALUE;
	float ax_y_max = _FAKE_VALUE;

	// 3. Самая левая и самая правая возможные точки эллипса?
	float lefter_point = e.front().x - ax_x_max;
	float righter_point = e.back().x + ax_x_max;

	// 4. Самая высокая и самая низкая возможные точки эллипса?
	// если найдем точку максимума, минимум оценим как самую низкую точку из возможных. Если найдем минимум - наоборот.
	float lower_point = _FAKE_VALUE;
	float heigher_point = _FAKE_VALUE;
	if (is_curve_increase) {
		for (int i = 1; i < e.size(); i++) {
			if (e[i - 1].y >= e[i].y) {
				heigher_point = e[i].y;// maximum
				if(e.front().y < e.back().y) ax_y_min = (e[i].y - e.front().y) / 2; else ax_y_min = (e[i].y - e.back().y) / 2;
					ax_y_max = ax_y_min * 4;
					lower_point = heigher_point - 2 * ax_y_max;
				break;
			}
		}
	}
	else {
		for (int i = 1; i < e.size(); i++) {
			if (e[i - 1].y <= e[i].y) {
				lower_point = e[i].y;// minimum
				if (e.front().y > e.back().y) ax_y_min = (e.front().y - e[i].y) / 2; else ax_y_min = (e.back().y - e[i].y) / 2;
					ax_y_max = ax_y_min * 4;
					heigher_point = lower_point + 2 * ax_y_max;
				break;
			}
		}
	}

	// 4.1. если не нашли макс или мин
	if ((lower_point == _FAKE_VALUE)&&(heigher_point == _FAKE_VALUE)) {
		if (is_curve_increase) {  
			ax_y_min = e.back().y - e.front().y;
			ax_y_max = ax_y_min * 4;
			lower_point = e.front().y - ax_y_max; 
			heigher_point = e.back().y + ax_y_max;
		}
		else { 
			ax_y_min = e.front().y - e.back().y;
			ax_y_max = ax_y_min * 4;
			lower_point = e.back().y - ax_y_max; 
			heigher_point = e.front().y + ax_y_max; 
		}
	}


	// 5. Перебор
	// значения, которые будем считать правдивыми (нормальными, хорошими)
	float center_x = _FAKE_VALUE, center_y = _FAKE_VALUE, cemiax_big = _FAKE_VALUE, cemiax_small = _FAKE_VALUE, decline = _FAKE_VALUE, excentricity = _FAKE_VALUE;
	float min_deflection = 400; // экспериментально оценили оптимальный вариант допустимого отклонения реального от идиального
	float deflection = min_deflection; 
	float acceccable_deflection = 1;
	float step = (ax_x_max + ax_y_max)/50, ang_step = 3.14/20; 

	float ax1_rez, ax2_rez;
	for (float ax_x = ax_x_min; ax_x < ax_x_max && deflection >= acceccable_deflection; ax_x += step) {
		for (float ax_y = ax_y_min; ax_y < ax_y_max && deflection >= acceccable_deflection; ax_y += step) {

			for (float x0 = lefter_point + ax_x; x0 < righter_point - ax_x && deflection >= acceccable_deflection; x0 += step) {
				for (float y0 = lower_point + ax_y; y0 < heigher_point - ax_y && deflection >= acceccable_deflection; y0 += step) {

					for (float dec = 0; dec < 3.14 && deflection >= acceccable_deflection; dec += ang_step) {

						deflection = 0;
						// эллипс: (x/ax1)^2 + (y/ax2)^2 = 1;
						// перебираем центральные точки, полуоси и наклоны (с учетом предполагаемых нами диапазонов). Подставляем в такое уравнение имеющиеся точки
						// при этом пользуемся тем, что: (x/ax1)^2 + (y/ax2)^2 < (x/ax1 + y/ax2)^2 - т.к. понимаем, что возведение в квадрат - дорогостоящая операция
						for (int i = 0; i < e.size(); i++) { // проходимся по всем точкам предполагаемого эллипса
							float cos_tmp = cos(dec), sin_tmp = sin(dec);
							deflection += abs(
								pow(((e.at(i).x - x0)*cos_tmp + (e.at(i).y - y0)*sin_tmp) / ax_x, 2) +
								pow(((e.at(i).x - x0)*(-sin_tmp) + (e.at(i).y - y0)*cos_tmp) / ax_y, 2) - 1
							);
						}
						if (deflection < min_deflection) {
							min_deflection = deflection;
							center_x = x0;
							center_y = y0;
							ax1_rez = ax_x;
							ax2_rez = ax_y;
							if (ax_x < ax_y) {
								cemiax_small = ax_x;
								cemiax_big = ax_y;
							}
							else {
								cemiax_small = ax_y;
								cemiax_big = ax_x;
							}
							decline = dec;

						}

					}

				}
			}


		}
	}

	// 6. Обрабатываем ситуацию, когда так ничего и не нашлось
	if(cemiax_small == _FAKE_VALUE){
		rez->Append(_FAKE_VALUE);
		rez_curve = rez;
		return;
	}

	// 7. Сохраняем результат
	excentricity = sqrt(1 - pow(cemiax_small / cemiax_big, 2));
	rez->Append(center_x);
	rez->Append(center_y);
	rez->Append(ax1_rez);
	rez->Append(ax2_rez);
	rez->Append(decline / 3.14 * 180);
	rez->Append(excentricity);

	rez_curve = rez;

	//	}); // async method
}



// for determiner find
void Calculator::permutate(vector<int>& a, int i1, int i2) {
	int i_tmp = a.at(i1);
	a.at(i1) = a.at(i2);
	a.at(i2) = i_tmp;
}

bool Calculator::anotherPermutation(vector<int>& cur_permut, int n) {
	int first_less = n - 2;
	while (first_less != -1 && cur_permut[first_less] >= cur_permut[first_less + 1])
		first_less--;
	if (first_less == -1)
		return false;
	int bigger_then_less = n - 1;
	while (cur_permut[first_less] >= cur_permut[bigger_then_less])
		bigger_then_less--;
	permutate(cur_permut, first_less, bigger_then_less);
	int l = first_less + 1, r = n - 1;
	while (l < r)
		permutate(cur_permut, l++, r--);
	return true;

}

vector<vector<int>> Calculator::allPermutations(int n) {
	vector<vector<int>> rezult;
	
	// первая комбинация (без каких-либо перестановок)
	vector<int> cur_permut;
	for (int i = 0; i < n; i++)
		cur_permut.push_back(i);
	rezult.push_back(cur_permut);

	while (anotherPermutation(cur_permut, n))
		rezult.push_back(cur_permut);

	return rezult;
}

short int Calculator::permutationParity(vector<int> permutation) {

	// четность числа инверсий в перестановке
	int inv_num = 0;
	for (int i = 0; i < permutation.size(); i++) {
		for (int j = i+1; j < permutation.size(); j++) {
			if (permutation[i] > permutation[j]) {
				inv_num += 1;
			}
		}
	}
	if (inv_num % 2 == 0)
		return 1;
	else
		return -1;
}

float Calculator::determinant(vector<vector<float>> matrix) {

	// для этого нужны всевозможные комбинации чисел от 1 до 6 без повторений	
	float rezult = 0;
	int n = matrix.size();
	vector<vector<int>> combinations = allPermutations(n);
	for (int i = 0; i < combinations.size(); i++) {
		float addend = 1;
		for (int j = 0; j < n; j++)
			addend *= matrix[j][combinations[i][j]];
		rezult += addend * permutationParity(combinations[i]);
	}
	return rezult;
}

vector<vector<float>> Calculator::replaceByUnicolumt(vector<vector<float>>mat_tmp, int replaced_col) {
	vector<vector<float>> rezult = mat_tmp;
	int n = mat_tmp.size();
	for (int i = 0; i < n; i++) {
		rezult[i][replaced_col] = 1;
	}
	return rezult;
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