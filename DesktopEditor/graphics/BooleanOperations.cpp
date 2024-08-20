#include "BooleanOperations.h"
#include <algorithm>

namespace Aggplus {
Segment::Segment() :
	P(PointD()),
	HI(PointD()),
	HO(PointD()),
	Id(0),
	Index(0) {}

Segment::Segment(const std::vector<PointD>& points, bool isCurve,
				 int index, int id, CGraphicsPath* path) :
	IsCurve(isCurve),
	Visited(false),
	Index(index), 
	Id(id),
	Path(path)
{
	if (IsCurve)
	{
		P = points[2];
		HI = PointD(points[0].X - P.X, points[0].Y - P.Y);
		HO = PointD(points[1].X - P.X, points[1].Y - P.Y);
	}
	else
		P = points[0];
}

Segment::Segment(const PointD& p, const PointD& hi, const PointD& ho) :
	P(p),
	HI(hi),
	HO(ho),
	IsCurve(false)
{
	if (!hi.Equals(PointD()) || !ho.Equals(PointD()))
		IsCurve = true;
}

bool Segment::IsValid(BooleanOpType op) const
{
	if (Visited)
		return false;
	if ((bool)Inters)
		return true;
	if (op == Subtraction)
	{
		if (Id == 1 && Wind->W == 0)
			return true;
		else if (Id == 2 && Wind->W == 1)
			return true;
	}
	else if (Wind->W == op)
		return true;
	return false;
}

bool Segment::Equals(const PointD& other) const
{
	return isZero(P.X - other.X) && isZero(P.Y - other.Y);
}

bool Segment::operator==(const Segment& other) const
{
	return (Index == other.Index) && (Id == other.Id);
}

bool Segment::operator!=(const Segment& other) const
{
	return !operator==(other);
}

Curve::Curve() :
	Segment1(Segment()),
	Segment2(Segment()){}

Curve::Curve(const Segment& segment1, const Segment& segment2) :
	Segment1(segment1),
	Segment2(segment2) {}

Curve::Curve(const std::vector<double>& values)
{
	if (values.size() == 4)
	{
		Segment1 = Segment(PointD(values[0], values[1]),
						   PointD(),
						   PointD());
		Segment2 = Segment(PointD(values[2], values[3]),
						   PointD(),
						   PointD());
	}
	else if (values.size() == 8)
	{
		Segment1 = Segment(PointD(values[0], values[1]),
						   PointD(),
						   PointD());
		Segment2 = Segment(PointD(values[6], values[7]),
						   PointD(values[2] - values[6],
								  values[3] - values[7]),
						   PointD(values[4] - values[6],
								  values[5] - values[7]));
	}
	else
	{
		Segment1 = Segment();
		Segment2 = Segment();
	}
}

Curve::Curve(double x0, double y0, double x1, double y1,
			 double x2, double y2, double x3, double y3)
{
	Segment1 = Segment(PointD(x0, y0), PointD(), PointD());
	Segment2 = Segment(PointD(x3, y3), PointD(x1, y1), PointD(x2, y2));
}

std::vector<double> Curve::GetXValues() const
{
	if (IsStraight())
		return {Segment1.P.X, Segment1.P.X, Segment2.P.X, Segment2.P.X};
	return {Segment1.P.X, Segment2.P.X + Segment2.HI.X,
			Segment2.P.X + Segment2.HO.X, Segment2.P.X};
}

std::vector<double> Curve::GetYValues() const
{
	if (IsStraight())
		return {Segment1.P.Y, Segment1.P.Y, Segment2.P.Y, Segment2.P.Y};
	return {Segment1.P.Y,
			Segment2.P.Y + Segment2.HI.Y,
			Segment2.P.Y + Segment2.HO.Y,
			Segment2.P.Y};
}

std::vector<double> Curve::GetPeeks() const
{
	std::vector<double>	x = GetXValues(),
						y = GetYValues(),
						roots;
	double	ax = -x[0] + 3 * x[1] - 3 * x[2] + x[3],
			bx =  3 * x[0] - 6 * x[1] + 3 * x[2],
			cx = -3 * x[0] + 3 * x[1],
			ay = -y[0] + 3 * y[1] - 3 * y[2] + y[3],
			by =  3 * y[0] - 6 * y[1] + 3 * y[2],
			cy = -3 * y[0] + 3 * y[1],
			tMin = CURVETIME_EPSILON,
			tMax = 1 - tMin;
	SolveCubic(9 * (ax * ax + ay * ay),
			   9 * (ax * bx + by * ay),
			   2 * (bx * bx + by * by) + 3 * (cx * ax + cy * ay),
			   (cx * bx + by * cy),
			   roots, tMin, tMax);
	std::sort(roots.begin(), roots.end());
	return roots;
}

double Curve::GetLength(double a, double b) const
{
	if(IsStraight())
	{
		Curve c = *this;
		if (b < 1)
			c = Subdivide(b)[0];
		if (a > 0)
			c = Subdivide(a)[1];
		double	dx = c.Segment2.P.X - c.Segment1.P.X,
				dy = c.Segment2.P.Y - c.Segment1.P.Y;
		return sqrt(dx * dx + dy * dy);
	}
	std::vector<double>	x = GetXValues(),
						y = GetYValues();
	double	ax = 9 * (x[1] - x[2]) + 3 * (x[3] - x[0]),
			bx = 6 * (x[0] + x[2]) - 12 * x[1],
			cx = 3 * (x[1] - x[0]),
			ay = 9 * (y[1] - y[2]) + 3 * (y[3] - y[0]),
			by = 6 * (y[0] + y[2]) - 12 * y[1],
			cy = 3 * (y[1] - y[0]);
	return integrate(ax, bx, cx, ay, by, cy, a, b);
}

double Curve::GetSquaredLineLength() const
{
	double	x = Segment2.P.X - Segment1.P.X,
			y = Segment2.P.Y - Segment1.P.Y;
	return x * x + y * y;
}

double Curve::GetTimeOf(const PointD& point) const
{
	PointD	p0 = Segment1.P,
			p3 = Segment2.P;
	double	d0 = getDistance(point.X, point.Y, p0.X, p0.Y),
			d3 = getDistance(point.X, point.Y, p3.X, p3.Y);

	if (d0 > EPSILON && d3 > EPSILON)
	{
		std::vector<double> coords = {point.X, point.Y},
							roots;
		for (int c = 0; c < 2; c++)
		{
			int count = SolveCubic(c, coords[c], roots, 0.0, 1.0);
			for (int i = 0; i < count; i++)
			{
				double u = roots[i];
				if (u > 1 || u < 0)
					return - 1;
				if (getDistance(point, GetPoint(u)) <= GEOMETRIC_EPSILON)
					return u;
			}
		}
	}
	bool firstDist = d0 <= GEOMETRIC_EPSILON,
		 secondDist = d3 <= GEOMETRIC_EPSILON;
	return firstDist ? 0 : secondDist ? 1 : - 1;
}

double Curve::GetTimeAt(double offset) const
{
	bool forward = offset > 0;
	double start = !forward ? 1 : 0;
	if (offset == 0)
		return start;

	double	a = forward ? start : 0,
			b = forward ? 1 : start,
			rangeLength = GetLength(a, b),
			diff = abs(offset) - rangeLength;

	if (abs(diff) < EPSILON)
		return forward ? b : a;
	else if (diff > EPSILON)
		return DBL_MIN;

	double	guess = offset / rangeLength,
			length = 0;
	std::vector<double>	x = GetXValues(),
						y = GetYValues();
	double	ax = 9 * (x[1] - x[2]) + 3 * (x[3] - x[0]),
			bx = 6 * (x[0] + x[2]) - 12 * x[1],
			cx = 3 * (x[1] - x[0]),
			ay = 9 * (y[1] - y[2]) + 3 * (y[3] - y[0]),
			by = 6 * (y[0] + y[2]) - 12 * y[1],
			cy = 3 * (y[1] - y[0]);
	return findRoot(length, start, offset, ax, bx, cx, 
					ay, by, cy, start + guess, a, b);
}

PointD Curve::Get(double t, int type) const
{
	std::vector<double>	x = GetXValues(),
						y = GetYValues();

	if (isZero(x[1] - x[0]) && isZero(y[1] - y[0]))
	{
		x[1] = x[0];
		y[1] = y[0];
	}
	if (isZero(x[2] - x[3]) && isZero(y[2] - y[3]))
	{
		x[2] = x[3];
		y[2] = y[3];
	}
	double	cx = 3 * (x[1] - x[0]),
			bx = 3 * (x[2] - x[1]) - cx,
			ax = x[3] - x[0] - cx - bx,
			cy = 3 * (y[1] - y[0]),
			by = 3 * (y[2] - y[1]) - cy,
			ay = y[3] - y[0] - cy - by,
			xv = ((ax * t + bx) * t + cx) * t + x[0],
			yv = ((ay * t + by) * t + cy) * t + y[0],
			x0 = t == 0 ? x[0] : t == 1 ? x[3] : xv,
			y0 = t == 0 ? y[0] : t == 1 ? y[3] : yv;

	if (type == 1)
	{
		double	tMin = CURVETIME_EPSILON,
				tMax = 1 - tMin;
	
		if (t < tMin)
		{
			x0 = cx;
			y0 = cy;
		}
		else if (t > tMax)
		{
			x0 = 3 * (x[3] - x[2]);
			y0 = 3 * (y[3] - y[2]);
		}
		else
		{
			x0 = (3 * ax * t + 2 * bx) * t + cx;
			y0 = (3 * ay * t + 2 * by) * t + cy;
		}

		if (x0 == 0.0 && y0 == 0.0 && (t < tMin || t > tMax))
		{
			x0 = x[2] - x[1];
			y0 = y[2] - y[1];
		}

		double len = sqrt(x0 * x0 + y0 * y0);
		if (len != 0.0)
		{
			x0 /= len;
			y0 /= len;
		}
	}
	return PointD(x0, y0);
}

PointD Curve::GetPoint(double t) const
{
	return Get(t, 0);
}

PointD Curve::GetTangent(double t) const
{
	return Get(t, 1);
}

PointD Curve::GetTangent(double t, double offset,
						 bool inside, const PointD& p) const
{
	if (inside)
		return GetTangent(t);
	PointD point =  GetPoint(GetTimeAt(offset));
	return PointD(point.X - p.X, point.Y - p.Y);
}

Curve Curve::GetPart(double from, double to) const
{
	Curve result = *this;
	if (from > to)
		std::swap(from, to);
	if (from > 0)
	{
		result = Subdivide(from)[1];
		result.Segment2.HI.X -= result.Segment2.P.X;
		result.Segment2.HI.Y -= result.Segment2.P.Y;
		result.Segment2.HO.X -= result.Segment2.P.X;
		result.Segment2.HO.Y -= result.Segment2.P.Y;
	}
	if (to < 1)
	{
		result = result.Subdivide((to - from) / (1 - from))[0];
		result.Segment2.HI.X -= result.Segment2.P.X;
		result.Segment2.HI.Y -= result.Segment2.P.Y;
		result.Segment2.HO.X -= result.Segment2.P.X;
		result.Segment2.HO.Y -= result.Segment2.P.Y;
	}
	if (from > to)
		result.Flip();
	return result;
}

std::vector<Curve> Curve::GetMonoCurves(bool dir) const
{
	std::vector<Curve>	curves;
	std::vector<double>	x = dir ? GetXValues() : GetYValues();
	bool cmpO1 = (x[0] >= x[1]) == (x[1] >= x[2]),
		 cmpO2 = (x[1] >= x[2]) == (x[2] >= x[3]);
	if ((cmpO1 && cmpO2) || IsStraight())
		curves.push_back(*this);
	else
	{
		std::vector<double> roots;
		double	a = 3 * (x[1] - x[2]) - x[0] + x[3],
				b = 2 * (x[0] + x[2]) - 4 * x[1],
				c = x[1] - x[0],
				tMin = CURVETIME_EPSILON,
				tMax = 1 - tMin,
				n = solveQuadratic(a, b, c, roots, tMin, tMax);
		if (n == 0)
			curves.push_back(*this);
		else
		{
			std::sort(roots.begin(), roots.end());
			double t = roots[0];
			std::vector<Curve> parts = Subdivide(t);
			curves.push_back(parts[0]);
			if (n > 1)
			{
				t = (roots[1] - t) / (1- t);
				parts = Subdivide(t);
				curves.push_back(parts[0]);
			}
			curves.push_back(parts[1]);
		}
	}
	return curves;
}

std::vector<std::pair<int, int>> Curve::GetOverlaps(const Curve& curve) const
{
	bool swap = this->GetSquaredLineLength() <
				curve.GetSquaredLineLength(),
		 straight1 = this->IsStraight(),
		 straight2 = curve.IsStraight(),
		 straightBoth = straight1 && straight2;
	std::vector<double>	x1 = GetXValues(),
						y1 = GetYValues(),
						x2 = curve.GetXValues(),
						y2 = curve.GetYValues();
	double	px = swap ? x2[0] : x1[0],
			py = swap ? y2[0] : y1[0],
			vx = swap ? x2[3] - px : x1[3] - px,
			vy = swap ? y2[3] - py : y1[3] - py;
	
	if (swap)
	{
		std::swap(x1, x2); 
		std::swap(y1, y2);
	}
	
	double	d20 = getDistance(px, py, vx, vy, x2[0], y2[0], true),
			d23 = getDistance(px, py, vx, vy, x2[3], y2[3], true);

	if (d20 < GEOMETRIC_EPSILON && d23 < GEOMETRIC_EPSILON)
	{
		double	d11 = getDistance(px, py, vx, vy, x1[1], y1[1], true),
				d12 = getDistance(px, py, vx, vy, x1[2], y1[2], true),
				d21 = getDistance(px, py, vx, vy, x2[1], y2[1], true),
				d22 = getDistance(px, py, vx, vy, x2[2], y2[2], true);

		if (!straightBoth &&
			d11 < GEOMETRIC_EPSILON &&
			d12 < GEOMETRIC_EPSILON &&
			d21 < GEOMETRIC_EPSILON &&
			d22 < GEOMETRIC_EPSILON)
		{
			straight1 = straight2 = straightBoth = true;
		}
	}
	else if (straightBoth)
	{
		return std::vector<std::pair<int, int>>();
	}
	if (straight1 ^ straight2)
	{
		return std::vector<std::pair<int, int>>();
	}

	std::vector<std::pair<int, int>> pairs;
	for (int i = 0; i < 4 && pairs.size() < 2; i++)
	{
		int i1 = i & 1,
			t1 = i >> 1;
		double t2 = i1 == 0	? GetTimeOf(t1 == 1	? curve.Segment2.P : curve.Segment1.P)
							: curve.GetTimeOf(t1 == 1 ? Segment2.P : Segment1.P);
		if (t2 != -1)
		{
			std::pair<int, int> pair = i1 == 1 ? std::pair<int, int>(t1, t2)
											   : std::pair<int, int>(t1, t2);
			if (pairs.empty())
				pairs.push_back(pair);
			else if (abs(pair.first - pairs[0].first) > TIME_EPSILON &&
					 abs(pair.second - pairs[0].second) > TIME_EPSILON)
			pairs.push_back(pair);
		}

		if (i > 2 && pairs.empty())
			break;
	}

	if (pairs.size() != 2)
		pairs.clear();
	else if (!straightBoth)
	{
		Curve	o1 = GetPart(pairs[0].first, pairs[1].first),
				o2 = curve.GetPart(pairs[0].second, pairs[1].second);
		double	dox = abs(o2.Segment2.HO.X - o1.Segment2.HO.X),
				doy = abs(o2.Segment2.HO.Y - o1.Segment2.HO.Y),
				dix = abs(o2.Segment2.HI.X - o1.Segment2.HI.X),
				diy = abs(o2.Segment2.HI.Y - o1.Segment2.HI.Y);
		if (dox > GEOMETRIC_EPSILON ||
			doy > GEOMETRIC_EPSILON ||
			dix > GEOMETRIC_EPSILON ||
			diy > GEOMETRIC_EPSILON)
		pairs.clear();
	}
	return pairs;
}

std::vector<double> Curve::GetCurveLineIntersection(double px, double py,
													double vx, double vy) const
{
	if (isZero(vx) && isZero(vy))
	{
		double t = GetTimeOf(PointD(px, py));
		return t == -1 ? std::vector<double>() : std::vector<double>({t});
	}

	double	angle = std::atan2(-vy, vx),
			sin = std::sin(angle),
			cos = std::cos(angle);
	std::vector<double>	x = GetXValues(),
						y = GetYValues(),
						rv, roots;
	for (int i = 0; i < 4; i++)
	{
		double cx = x[i] - px,
			   cy = y[i] - py;
		rv.push_back(cx * cos - cy * sin);
		rv.push_back(cx * sin + cy * cos);
	}
	Curve curve = Curve(rv);
	curve.SolveCubic(1, 0, roots, 0, 1);
	return roots;
}

std::vector<Curve> Curve::Subdivide(double t) const
{
	std::vector<double>	x = GetXValues(),
						y = GetYValues();
	double x2[6], y2[6], u = 1 - t;

	x2[0] = u * x[0] + t * x[1], y2[0] = u * y[0] + t * y[1],
	x2[1] = u * x[1] + t * x[2], y2[1] = u * y[1] + t * y[2],
	x2[2] = u * x[2] + t * x[3], y2[2] = u * y[2] + t * y[3],
	x2[3] = u * x2[0] + t * x2[1], y2[3] = u * y2[0] + t * y2[1],
	x2[4] = u * x2[1] + t * x2[2], y2[4] = u * y2[1] + t * y2[2],
	x2[5] = u * x2[3] + t * x2[4], y2[5] = u * y2[3] + t * y2[4];

	return {Curve(x[0], y[0], x2[0], y2[0], x2[3], y2[3], x2[5], y2[5]),
			Curve(x2[5], y2[5], x2[4], y2[4], x2[2], y2[2], x[3], y[3])};
}

Curve Curve::DivideAtTime(double time, std::vector<Segment>& segments,
						  std::vector<Curve>& curves)
{
	double	tMin = CURVETIME_EPSILON,
			tMax = 1 - tMin;

	if (time >= tMin && time <= tMax)
	{
		std::vector<Curve> parts = Subdivide(time);
		Curve	left = parts[0],
				right = parts[1];

		std::vector<PointD> points({left.Segment2.P});
		if (!IsStraight())
			points.insert(points.begin(), {PointD(), PointD()});
		Segment segment = Segment(points,
								  !IsStraight(),
								  Segment1.Index + 1,
								  Segment1.Id,
								  Segment1.Path);

		segment.HI = PointD(left.Segment2.HI.X - left.Segment2.P.X,
							left.Segment2.HI.Y - left.Segment2.P.Y);
		segment.HO = PointD(left.Segment2.HO.X - left.Segment2.P.X,
							 left.Segment2.HO.Y - left.Segment2.P.Y);
		Segment2.HI = PointD(right.Segment2.HI.X - right.Segment2.P.X,
							 right.Segment2.HI.Y - right.Segment2.P.Y);
		Segment2.HO = PointD(right.Segment2.HO.X - right.Segment2.P.X,
							 right.Segment2.HO.Y - right.Segment2.P.Y);
		Segment tmp = Segment2;
		Segment2 = segment;
		return Curve(segment, tmp);
	}
	return Curve();
}

int Curve::SolveCubic(int coord, double value, std::vector<double>& roots,
					double mn, double mx) const
{
	int count = 0;
	std::vector<double> v = coord == 0 ? GetXValues() : GetYValues();
	double a, b, c, d;

	if (!((v[0] < value && v[3] < value && v[1] < value && v[2] < value) ||
		  (v[0] > value && v[3] > value && v[1] > value && v[2] > value)))
	{
		c = 3 * (v[1] - v[0]);
		b = 3 * (v[2] - v[1]) - c;
		a = v[3] - v[0] - c - b;
		d = v[0] - value;
		count = SolveCubic(a, b, c, d, roots, mn, mx);
	}
	return count;
}

int Curve::SolveCubic(double a, double b, double c, double d,
					std::vector<double>& roots, double mn, double mx) const
{
	int count = 0;
	double f = max(abs(a), abs(b), abs(c), abs(d));
	double x, b1, c2, qd, q;
	if (f < 1e-8 || f > 1e8)
	{
		f = pow(2, -round(log2(f)));
		a *= f;
		b *= f;
		c *= f;
		d *= f;
	}

	if (abs(a) < EPSILON)
	{
		a = b;
		b1 = c;
		c2 = d;
		x = DBL_MAX;
	}
	else if (abs(d) <EPSILON)
	{
		b1 = b;
		c2 = c;
		x = 0;
	}
	else
	{
		x = -(b / a) / 3;
		b1 = a * x + b;
		c2 = b1 * x + c;
		qd = (a * x + b1) * x + c2;
		q = c2 * x + d;
		double t = q / a,
			  r = pow(abs(t), 1 / 3),
			  s = t < 0 ? -1 : 1,
			  td = -qd / a,
			  rd = td > 0 ? 1.324717957244746 * std::max(r, sqrt(td)) : r,
			  x0 = x - s * rd;
		if (x0 != x)
		{
			do
			{
				x = x0;
				b1 = a * x + b;
				c2 = b1 * x + c;
				qd = (a * x + b1) * x + c2;
				q = c2 * x + d;
				x0 = qd == 0 ? x : x - q / qd / (1 + MACHINE_EPSILON);
			} while (s * x0 > s * x);

			if (abs(a) * x * x > abs(d / x))
			{
				c2 = -d / x;
				b1 = (c2 - c) / x;
			}
		}
	}
	count = solveQuadratic(a, b1, c2, roots, mn, mx);
	bool xInRoots1 = count > 0 && x != roots[0],
		 xInRoots2 = count > 1 && x != roots[1],
		 xInEps = x > mn - EPSILON && x < mx + EPSILON;
	if (x != DBL_MAX && (count == 0 || xInRoots1 || xInRoots2) && (xInEps))
	{
		roots.push_back(clamp(x, mn, mx));
		count++;
	}
	return count;
}

void Curve::Flip()
{
	std::swap(Segment1.P, Segment2.P);
	std::swap(Segment1.HI, Segment1.HO);
}

void Curve::ClearHandles()
{
	Segment2.HI = PointD();
	Segment2.HO = PointD();
}

bool Curve::HasHandle() const
{
	return	!isZero(Segment2.HI.X) ||
			!isZero(Segment2.HI.Y) ||
			!isZero(Segment2.HO.X) ||
			!isZero(Segment2.HO.Y);
}

bool Curve::IsStraight() const
{
	return !Segment2.IsCurve;
}

bool Curve::operator==(const Curve& other) const
{
	return Segment1 == other.Segment1 && 
		   Segment2 == other.Segment2;
}

bool Curve::operator!=(const Curve& other) const
{
	return !operator ==(other);
}

Location::Location() :
	C(Curve()),
	S(Segment()),
	Inters(nullptr),
	Next(nullptr),
	Prev(nullptr) {}

Location::Location(const Curve& curve, double time, bool overlap) :
	C(curve),
	Time(time),
	Overlap(overlap) {}

bool Location::IsTouching()
{
	if (this->Inters == nullptr)
		return false;
	if (!isCollinear(C.GetTangent(Time), Inters->C.GetTangent(Inters->Time)))
		return false;
	Curve	c1 = this->C,
			c2 = this->Inters->C;
	bool straight = c1.IsStraight() && c2.IsStraight();
	return !straight || intersect(c1.Segment1.P.X,
								  c1.Segment1.P.Y,
								  c1.Segment2.P.X,
								  c1.Segment2.P.Y,
								  c2.Segment1.P.X,
								  c2.Segment1.P.Y,
								  c2.Segment2.P.X,
								  c2.Segment2.P.Y).Equals(PointD());
}

bool Location::operator==(const Location& other)
{
	return isZero(C.GetPoint(Time).X - other.C.GetPoint(other.Time).X) &&
		   isZero(C.GetPoint(Time).Y - other.C.GetPoint(other.Time).Y) &&
		   C.Segment1.Id == other.C.Segment1.Id;
}

Branch::Branch() :
	Start(0.0),
	HI(PointD()) {}

Branch::Branch(double start, const std::vector<Segment>& crossings, const PointD& hi) :
	Start(start),
	Crossings(crossings),
	HI(hi) {}

Winding::Winding() :
	Prev(Curve()),
	OnPath(false),
	OnAnyPath(false),
	DontFlip(false),
	W(0.0),
	Quality(1.0),
	WindingLeft(0.0),
	WindingRight(0.0),
	PathWindingLeft(0.0),
	PathWindingRight(0.0) {}

void Winding::Copy(Winding* wind)
{
	OnAnyPath		= wind->OnAnyPath;
	W				= wind->W;
	Quality			= wind->Quality;
	WindingLeft 	= wind->WindingLeft;
	WindingRight	= wind->WindingRight;
}

void Winding::Reset()
{
	OnPath = false;
	OnAnyPath = false;
	Roots.clear();
	Prev = Curve();
}

void Winding::SetDirection(bool direction)
{
	Direction = direction;
}

void Winding::SetPoint(const PointD& point)
{
	PointAbscissas		= Direction ? point.Y : point.X;
	PointOrdinat		= Direction ? point.X : point.Y;
	PointAbscissasLeft	= PointAbscissas - WINDING_EPSILON;
	PointAbscissasRight	= PointAbscissas + WINDING_EPSILON;
}

void Winding::SetCurves(const std::vector<Curve>& curvesHor, const std::vector<Curve>& curvesVer)
{
	Curves = Direction ? curvesHor : curvesVer;
	CurvesHor = curvesHor;
	CurvesVer = curvesVer;
}

Winding* Winding::AddWinding(const Curve& curve)
{
	std::vector<double>	x = Direction ? curve.GetXValues() : curve.GetYValues(),
						y = Direction ? curve.GetYValues() : curve.GetXValues(),
						xPrev = Prev.GetXValues(),
						yPrev = Prev.GetYValues();

	if (PointOrdinat < std::min(x[0], x[3]) ||
		PointOrdinat > std::max(x[0], x[3]))
		return nullptr;

	if (x[0] == x[3])
	{
		bool a0InRange = y[0] < PointAbscissasRight &&
						 y[3] > PointAbscissasLeft,
			 a3InRange = y[3] < PointAbscissasRight &&
						 y[0] > PointAbscissasLeft;
		if (a0InRange || a3InRange)
			OnPath = true;
		return nullptr;
	}

	double t;
	if (PointOrdinat == x[0] || PointOrdinat == x[3])
		t = PointOrdinat == x[0] ? 0.0 : 1.0;
	else if (PointAbscissasLeft > max(y[0], y[1], y[2], y[3]) ||
			 PointAbscissasRight < min(y[0], y[1], y[2], y[3]))
		t = 1.0;
	else
	{
		int count =	curve.SolveCubic(Direction ? 0 : 1, PointOrdinat,
									 Roots, 0, 1);
		t = count > 0 ? Roots[0] : 1.0;
	}

	double abscis;
	if (t == 0.0 || t == 1.0)
		abscis = t == 0.0 ? y[0] : y[3];
	else
	{
		PointD pt = curve.GetPoint(t);
		abscis = Direction ? pt.Y : pt.X;
	}

	double	winding = x[0] > x[3] ? 1.0 : -1.0;
	bool	dirDiff = Direction ? xPrev[0] > xPrev[3] : yPrev[0] > yPrev[3];
	double	windingPrev = dirDiff ? 1.0 : -1.0;
	double	a3Prev = Direction ? yPrev[3] : xPrev[3];

	if (PointOrdinat != x[0])
	{
		if (abscis < PointAbscissasLeft)
			PathWindingLeft += winding;
		else if (abscis > PointAbscissasRight)
			PathWindingRight += winding;
		else
			OnPath = true;

		if (abscis > (PointAbscissas - QUALITI_EPSILON) &&
			abscis < (PointAbscissas + QUALITI_EPSILON))
			Quality /= 2;
	}
	else
	{
		if (winding != windingPrev)
		{
			if (y[0] < PointAbscissasLeft)
				PathWindingLeft += winding;
			else if (y[0] > PointAbscissasRight)
				PathWindingRight += winding;
		}
		else if (y[0] != a3Prev)
		{
			if (a3Prev < PointAbscissasRight && abscis > PointAbscissasRight)
			{
				PathWindingRight += winding;
				OnPath = true;
			}
			else if (a3Prev > PointAbscissasLeft && abscis < PointAbscissasLeft)
			{
				PathWindingLeft += winding;
				OnPath = true;
			}
		}
		Quality /= 4;
	}
	Prev = curve;

	PointD	pt = curve.GetTangent(t);
	bool	inRange = abscis > PointAbscissasLeft &&
					  abscis < PointAbscissasRight &&
					  (Direction ? pt.X : pt.Y) == 0.0;
	if (DontFlip || !inRange)
		return nullptr;

	Winding* newWind	= new Winding;
	newWind->DontFlip	= true;
	newWind->SetDirection(!Direction);
	newWind->SetPoint(PointD(PointAbscissas, PointOrdinat));
	newWind->SetCurves(CurvesHor, CurvesVer);
	newWind->Curves1 = Curves1;
	newWind->Curves2 = Curves2;
	newWind->GetWinding();
	return newWind;
}

Winding* Winding::HandleCurve(const Curve& curve)
{
	std::vector<double>	x = Direction ? curve.GetXValues() : curve.GetYValues(),
						y = Direction ? curve.GetYValues() : curve.GetXValues();

	if (PointOrdinat <= max(x[0], x[1], x[2], x[3]) &&
		PointOrdinat >= min(x[0], x[1], x[2], x[3]))
	{
		std::vector<Curve> monoCurves;
		if (PointAbscissasLeft > max(y[0], y[1], y[2], y[3]) ||
			PointAbscissasRight < min(y[0], y[1], y[2], y[3]))
			monoCurves.push_back(curve);
		else
			monoCurves = curve.GetMonoCurves(Direction);
		for (const auto& c : monoCurves)
		{
			Winding* wind = AddWinding(c);
			if (wind != nullptr)
				return wind;
		}
	}
	return nullptr;
}

void Winding::GetWinding()
{
	for (int i = 0; i < Curves.size(); i++)
	{
		Curve close;
		if (i == 0 || Curves[i - 1].Segment1.Id != Curves[i].Segment1.Id)
		{
			Prev = Curve();
			bool path1 = Curves[i].Segment1.Id == 1;
			if (!Curves[i].Segment1.Path->Is_poly_closed())
			{
				close = path1 ? Curve(Curves1[Curves1.size() - 1].Segment2, Curves[i].Segment1)
							  : Curve(Curves2[Curves2.size() - 1].Segment2, Curves[i].Segment1);
				std::vector<double> x = close.GetXValues(),
									y = close.GetYValues();
				if ((Direction ? x[0] : y[0]) != (Direction ? x[3] : y[3]))
					Prev = close;
			}

			if (Prev == Curve())
			{
				Prev = Curves[i];
				Curve prev = path1 ? Curves1[Curves1.size() - 1]
								   : Curves2[Curves2.size() - 1];
				while (prev != Curve() && prev != Curves[i])
				{
					std::vector<double>	x = prev.GetXValues(),
										y = prev.GetYValues();

					if ((Direction ? x[0] : y[0]) != (Direction ? x[3] : y[3]))
					{
						Prev = prev;
						break;
					}
					prev = GetPreviousCurve(prev);
				}
			}
		}

		Winding* wind = HandleCurve(Curves[i]);
		if (wind != nullptr)
		{
			this->Copy(wind);
			return;
		}

		if (i + 1 == Curves.size() || Curves[i + 1].Segment1.Id != Curves[i].Segment1.Id)
		{
			if (close != Curve())
			{
				Winding* wind1 = HandleCurve(close);
				if (wind1 != nullptr)
				{
					this->Copy(wind);
					return;
				}
			}

			if (OnPath && PathWindingLeft == 0.0 && PathWindingRight == 0.0)
				PathWindingLeft = PathWindingRight =
					Curves[i].Segment1.Path->IsClockwise() ^ Direction ? 1.0 : -1.0;

			WindingLeft += PathWindingLeft;
			WindingRight += PathWindingRight;
			PathWindingLeft = PathWindingRight = 0.0;
			if (OnPath)
			{
				OnAnyPath = true;
				OnPath = false;
			}
		}
	}

	WindingLeft = abs(WindingLeft);
	WindingRight = abs(WindingRight);

	return;
}

CBooleanOperations::CBooleanOperations(CGraphicsPath* path1,
									CGraphicsPath* path2,
									BooleanOpType op) :
	Op(op),
	Path1(path1),
	Path2(path2),
	Result(new CGraphicsPath)
{
	TraceBoolean();
}

CGraphicsPath *CBooleanOperations::GetResult()
{
	return Result;
}

int CBooleanOperations::CheckInters(const PointD& point, const Segment& segment, const Curve& curve) const
{
	PointD pt = intersect(point.X, point.Y, segment.P.X, segment.P.Y, curve.Segment1.P.X, curve.Segment1.P.Y, curve.Segment2.P.X, curve.Segment2.P.Y);
	if (curve.Segment1.Equals(pt) || curve.Segment2.Equals(pt))
	{
		PointD newPoint = PointD(point.X + GEOMETRIC_EPSILON, point.Y);
		return CheckInters(newPoint, segment, curve);
	}
	else if (!pt.Equals(PointD()) && curve.IsStraight())
	{
		return 1;
	}
	else if (!curve.IsStraight())
	{
		std::vector<double> roots = curve.GetCurveLineIntersection(point.X, point.Y,
																   segment.P.X - point.X,
																   segment.P.Y - point.Y);
		Curve line = Curve(segment, Segment(point, PointD(), PointD()));
		int count = 0;
		for (const auto& r : roots)
			if (line.GetTimeOf(curve.GetPoint(r)) != -1)
				count++;
		return count;
	}
	return 0;
}

void CBooleanOperations::TraceBoolean()
{
	bool reverse = false;
	if ((Op == Subtraction || Op == Exclusion) ^
		Path1->IsClockwise() ^
		Path2->IsClockwise())
		reverse = true;

	PreparePath(Path1, 1, Segments1, Curves1);
	PreparePath(Path2, 2, Segments2, Curves2, reverse);

	GetIntersection();
	
	int length = Locations.size() - 1;
	for (int i = length; i >= 0; i--)
	{
		InsertLocation(Locations[i]->Inters, false);
	}

	DivideLocations();

	if (Locations.empty())
		return;

	for (const auto& l : Locations)
	{
		Winding winding;
		Segment start = l->S,
			s = GetNextSegment(l->S);

		// if (getDistance(s.P, l->S.P) < GEOMETRIC_EPSILON)
		// {
		// 	winding.W = 1;
		// 	if (s.Id == 1 )
		// 		Segments1[s.Index].Wind = std::make_shared<Winding>(winding);
		// 	else
		// 		Segments2[s.Index].Wind = std::make_shared<Winding>(winding);
		// 	s = GetNextSegment(s);
		// }

		if (s == Segment() || (bool)s.Inters || s == start)
			continue;

		int count = 0;
		for (const auto& c : (s.Id == 1 ? Curves2 : Curves1))
			count += CheckInters(PointD(), s, c);

		winding.W = count % 2;

		do
		{
			if (s.Id == 1 )
				Segments1[s.Index].Wind = std::make_shared<Winding>(winding);
			else
				Segments2[s.Index].Wind = std::make_shared<Winding>(winding);
			s = GetNextSegment(s);
		} while (s != Segment() && !(bool)s.Inters && s != start);
	}

	Segments = Segments1;
	Segments.insert(Segments.end(), Segments2.begin(), Segments2.end());
	Curves = Curves1;
	Curves.insert(Curves.end(), Curves2.begin(), Curves2.end());

	TracePaths();
}

void CBooleanOperations::SetVisited(const Segment& segment)
{
	if (segment.Id == 1)
	{
		Segments1[segment.Index].Visited = true;
		Segments[segment.Index].Visited = true;
	}
	else
	{
		Segments2[segment.Index].Visited = true;
		Segments[segment.Index + Segments1.size()].Visited = true;
	}
}

void CBooleanOperations::TracePaths()
{
	Result->StartFigure();
	for (auto& s : Segments)
	{
		bool valid = s.IsValid(Op),
			 start = true;
		while (valid)
		{
			if (Op != Union && Op != Subtraction || !start)
				SetVisited(s);
			if (start)
			{
				Result->MoveTo(s.P.X, s.P.Y);
				start = false;
			}
			else
			{
				if (s.IsCurve)
					Result->CurveTo(s.HI.X + s.P.X, s.HI.Y + s.P.Y, s.HO.X + s.P.X, s.HO.Y + s.P.Y, s.P.X, s.P.Y);
				else
					Result->LineTo(s.P.X, s.P.Y);
			}

			Segment prev = s;
			s = GetNextSegment(s);
			valid = s.IsValid(Op);
			if (s.Inters && prev.Inters && Op != Intersection ||
				!valid && prev.Inters)
			{
				s = GetNextSegment(prev.Inters->S);
				SetVisited(prev.Inters->S);
				valid = s.IsValid(Op);
			}
		}
	}
	Result->CloseFigure();
}

void CBooleanOperations::PreparePath(CGraphicsPath* path, int id,
									 std::vector<Segment>& segments,
									 std::vector<Curve>& curves,
									 bool reverse)
{
	int length = path->Is_poly_closed() ? (path->GetPointCount() - 1) : path->GetPointCount();
	int idx = 0;
	if (reverse)
	{
		bool isCurve = false;
		for (int i = length; i > 0; i--)
		{
			std::vector<PointD> points = path->GetPoints(isCurve ? i - 2 : i, isCurve ? 3 : 1);
			std::reverse(points.begin(), points.end());
			if (isCurve && i - 2 == 0)
			{
				segments[0].IsCurve = true;
				segments[0].HI = PointD(points[0].X - segments[0].P.X, points[0].Y - segments[0].P.Y);
				segments[0].HO = PointD(points[1].X - segments[0].P.X, points[1].Y - segments[0].P.Y);
			}
			else
				segments.push_back(Segment(points, isCurve, idx++, id, path));
			if (isCurve) i -= 2;
			isCurve = path->IsCurvePoint(i);
		}
	}
	else
	{
		for (int i = 0; i < length; i++)
		{
			bool isCurve = path->IsCurvePoint(i);
			std::vector<PointD> points = path->GetPoints(i, isCurve ? 3 : 1);
			if (isCurve && i + 2 == length)
			{
				segments[0].IsCurve = true;
				segments[0].HI = PointD(points[0].X - segments[0].P.X, points[0].Y - segments[0].P.Y);
				segments[0].HO = PointD(points[1].X - segments[0].P.X, points[1].Y - segments[0].P.Y);
			}
			else
				segments.push_back(Segment(points, isCurve, idx++, id, path));
			if (isCurve) i += 2;
		}
	}
	length = segments.size();
	for (int i = 0; i < length; i++)
	{
		curves.push_back(Curve(segments[i], i == (length - 1) ? segments[0] : segments[i + 1]));
	}
}

void CBooleanOperations::InsertSegment(const Segment& segment, const Segment& handles, bool updateHandles)
{
	if (segment.Id == 1)
	{
		int length = Segments1.size(),
			index = segment.Index;
		if (segment.P.Equals(Segments1[index == length ? 0 : index].P) &&
			segment.HI.Equals(Segments1[index == length ? 0 : index].HI) &&
			segment.HO.Equals(Segments1[index == length ? 0 : index].HO))
		{
			Segments1[index == length ? 0 : index] = segment;
			return;
		}
		Segments1.insert(Segments1.begin() + index, segment);
		for (int i = index + 1; i < length + 1; i++)
		{
			Segments1[i].Index++;
		}
		if (updateHandles)
		{
			Segments1[index == length ? 0 : index + 1].HI = handles.HI;
			Segments1[index == length ? 0 : index + 1].HO = handles.HO;
		}
		Curves1.clear();
		for (int i = 0; i < length + 1; i++)
		{
			Curves1.push_back(Curve(Segments1[i], i == (Segments1.size() - 1) ?
													  Segments1[0] :
													  Segments1[i + 1]));
		}
	}
	else
	{
		int length = Segments2.size(),
			   index = segment.Index;
		if (segment.P.Equals(Segments2[index == length ? 0 : index].P) &&
			segment.HI.Equals(Segments2[index == length ? 0 : index].HI) &&
			segment.HO.Equals(Segments2[index == length ? 0 : index].HO))
		{
			Segments2[index == length ? 0 : index] = segment;
			return;
		}
		Segments2.insert(Segments2.begin() + index, segment);
		for (int i = segment.Index + 1; i < length + 1; i++)
		{
			Segments2[i].Index++;
		}
		if (updateHandles)
		{
			Segments2[index == length ? 0 : index + 1].HI = handles.HI;
			Segments2[index == length ? 0 : index + 1].HO = handles.HO;
		}
		Curves2.clear();
		for (int i = 0; i < length + 1; i++)
		{
			Curves2.push_back(Curve(Segments2[i], i == (Segments2.size() - 1) ?
													  Segments2[0] :
													  Segments2[i + 1]));
		}
	}

	for (auto& l : Locations)
		if (l->S.Index >= segment.Index && l->S.Id == segment.Id)
			l->S.Index++;
}

Curve CBooleanOperations::GetCurve(const Segment& segment) const
{
	bool path1 = segment.Id == 1;
	if (segment.Path->Is_poly_closed() &&
		segment.Index == (path1 ? Segments1.size() - 1 : Segments2.size() - 1))
		return path1 ? Curves1[segment.Index - 1] : Curves2[segment.Index - 1];
	return path1 ? Curves1[segment.Index] : Curves2[segment.Index];
}

Curve Winding::GetPreviousCurve(const Curve& curve) const
{
	int index = curve.Segment1.Index;
	if (index == 0)
		return Curve();
	return curve.Segment1.Id == 1 ? Curves1[index - 1] : Curves2[index - 1];
}

Curve CBooleanOperations::GetPreviousCurve(const Curve& curve) const
{
	bool path1 = curve.Segment1.Id == 1;
	int length = path1 ? Curves1.size()
						: Curves2.size();
	if (curve.Segment1.Index == 0)
	{
		if (curve.Segment1.Path->Is_poly_closed())
			return path1 ? Curves1[length - 1] : Curves2[length - 1];
		else
			return Curve();
	}
	return	path1 ? Curves1[curve.Segment1.Index - 1]
				  : Curves2[curve.Segment1.Index - 1];
}

Curve CBooleanOperations::GetNextCurve(const Curve& curve) const
{
	bool path1 = curve.Segment1.Id == 1;
	int last = path1 ? Curves1.size() - 1
						: Curves2.size() - 1;
	if (curve.Segment1.Index == last)
	{
		if (curve.Segment1.Path->Is_poly_closed())
			return path1 ? Curves1[0] : Curves2[0];
		else
			return Curve();
	}
	return	path1 ? Curves1[curve.Segment1.Index + 1]
				  : Curves2[curve.Segment1.Index + 1];
}

Segment CBooleanOperations::GetNextSegment(const Segment& segment) const
{
	bool path1 = segment.Id == 1;
	int length = path1 ? Segments1.size() : Segments2.size();
	if (segment.Index == (length - 1))
	{
		if (segment.Path->Is_poly_closed())
			return path1 ? Segments1[0] : Segments2[0];
		else
			return Segment();
	}
	return path1 ? Segments1[segment.Index + 1] : Segments2[segment.Index + 1];
}

std::vector<Curve> CBooleanOperations::GetCurves(const std::vector<int>& indices) const
{
	std::vector<Curve> list;
	for (const auto& i : indices)
		list.push_back(Curves[i]);
	return list;
}

std::vector<Segment> CBooleanOperations::GetCrossing(const Segment& seg,
													std::vector<Segment>& starts,
													bool collectStart) const
{
	std::vector<Segment> crossing;
	std::shared_ptr<Location>	loc = seg.Inters,
								start = loc;
	
	if (collectStart)
		starts = {seg};

	if (loc != nullptr)
	{
		Collect(seg, crossing, starts, loc, nullptr, collectStart);
		loc = start;

		while (loc != nullptr && loc->Prev != nullptr)
			loc = loc->Prev;
		Collect(seg, crossing, starts, loc, start, collectStart);
	}
	return crossing;
}

std::vector<std::vector<double>> CBooleanOperations::GetBoundsForCurves(
	const std::vector<Curve>& curves) const
{
	std::vector<std::vector<double>> bounds;
	for (int i = 0; i < curves.size(); i++)
	{
		std::vector<double>	x = curves[i].GetXValues(),
							y = curves[i].GetYValues();
		bounds.push_back({min(x[0], x[1], x[2], x[3]),
						  min(y[0], y[1], y[2], y[3]),
						  max(x[0], x[1], x[2], x[3]),
						  max(y[0], y[1], y[2], y[3])});
	}
	return bounds;
}

std::vector<std::vector<int>> CBooleanOperations::FindCurveBoundsCollisions(
	const std::vector<Curve>& curves1,
	const std::vector<Curve>& curves2,
	double tolerance,
	bool bothAxis) 
{
	std::vector<std::vector<double>> bounds1 = GetBoundsForCurves(curves1);
	std::vector<std::vector<double>> bounds2 = GetBoundsForCurves(curves2);

	if (bothAxis)
	{
		std::vector<std::vector<int>> hor = 
			FindBoundsCollisions(bounds1, bounds2, tolerance, false, true);
		std::vector<std::vector<int>> ver = 
			FindBoundsCollisions(bounds1, bounds2, tolerance, true, true);
		std::vector<std::vector<int>> list;
		for (int i = 0; i < hor.size(); i++)
		{
			list.push_back(hor[i]);
			list.push_back(ver[i]);
		}
		return list;
	}
	return FindBoundsCollisions(bounds1, bounds2, tolerance);
}

std::vector<std::vector<int>> CBooleanOperations::FindBoundsCollisions(
	const std::vector<std::vector<double>>& bounds1,
	const std::vector<std::vector<double>>& bounds2,
	double tolerance,
	bool sweepVertical,
	bool onlySweep)
{
	bool self = bounds1 == bounds2;

	std::vector<std::vector<double>> allBounds(bounds1);
	if (!self)
		std::copy(bounds2.begin(), bounds2.end(), std::back_inserter(allBounds));
	int	allLength = allBounds.size(),
			length1 = bounds1.size();

	int	pri1 = sweepVertical ? 1 : 0,
			pri2 = pri1 + 2,
			sec1 = sweepVertical ? 0 : 1,
			sec2 = sec1 + 2;

	std::vector<int> allIdicesByPri1(allLength);
	for (int i = 0; i < allLength; i++)
	{
		allIdicesByPri1[i] = i;
	}
	std::sort(allIdicesByPri1.begin(),
			  allIdicesByPri1.end(),
	[&allBounds, &pri1](int i1, int i2){
				  return allBounds[i1][pri1] < allBounds[i2][pri1];
	});

	std::vector<int> activeIndicesByPri2;
	std::vector<std::vector<int>> allCollisions(length1);
	for (int i = 0; i < allLength; i++)
	{
		int curIndex = allIdicesByPri1[i];
		std::vector<double> curBounds = allBounds[curIndex];
		std::vector<int> curCollisions;
		bool isCurrent1 = curIndex < length1,
			 isCurrent2 = self || !isCurrent1;
		int origIndex = self ? curIndex : curIndex - length1;
		if (!activeIndicesByPri2.empty())
		{
			int pruneCount =
				binarySearch(allBounds, activeIndicesByPri2, pri2,
							 curBounds[pri1] - tolerance) + 1;
			activeIndicesByPri2.erase(activeIndicesByPri2.begin(), 
									  activeIndicesByPri2.begin() + pruneCount);
			if (self && onlySweep)
			{
				if (isCurrent1)
					curCollisions.insert(curCollisions.end(),
										 activeIndicesByPri2.begin(),
										 activeIndicesByPri2.end());
				for (int j = 0; j < activeIndicesByPri2.size(); j++)
				{
					int activeIndex = activeIndicesByPri2[j];
					allCollisions[activeIndex].push_back(origIndex);
				}
			}
			else
			{
				double	curSec2 = curBounds[sec2],
						curSec1 = curBounds[sec1];
				for (int j = 0; j < activeIndicesByPri2.size(); j++)
				{
					int activeIndex = activeIndicesByPri2[j];
					std::vector<double> activeBounds = allBounds[activeIndex];
					bool isActive1 = activeIndex < length1,
						isActive2 = self || !isActive1,
						isActive1Or2 = ((isCurrent1 && isActive2) || (isCurrent2 && isActive1)),
						inRange1 = curSec1 <= activeBounds[sec2] + tolerance,
						inRange2 = curSec2 >= activeBounds[sec1] - tolerance;
					if (onlySweep || (isActive1Or2 && (inRange2 && inRange1)))
					{
						if (isCurrent1 && isActive2)
							curCollisions.push_back(self ? activeIndex : activeIndex - length1);
						if (isCurrent2 && isActive1)
							allCollisions[activeIndex].push_back(origIndex);
					}
				}
			}
		}
		if (isCurrent1)
		{
			if (bounds1 == bounds2)
				curCollisions.push_back(curIndex);
			allCollisions[curIndex] = curCollisions;
		}
		if (activeIndicesByPri2.size() > 0)
		{
			double curPri2 = curBounds[pri2];
			int index = binarySearch(allBounds, activeIndicesByPri2, pri2, curPri2);
			activeIndicesByPri2.insert(activeIndicesByPri2.begin() + (1 + index), curIndex);
		}
		else
		{
			activeIndicesByPri2.push_back(curIndex);
		}
	}
	for (auto& c : allCollisions)
	{
		if (!c.empty())
		std::sort(c.begin(), c.end());
	}
	return allCollisions;
}

bool CBooleanOperations::IsCrossing(std::shared_ptr<Location> loc)
{
	if(loc->Inters == nullptr)
		return false;

	double	t1 = loc->Time,
			t2 = loc->Inters->Time,
			tMin = CURVETIME_EPSILON,
			tMax = 1 - tMin;
	bool	t1Inside = t1 >= tMin && t1 <= tMax,
			t2Inside = t2 >= tMin && t2 <= tMax;

	if (t1Inside && t2Inside)
		return !loc->IsTouching();

	Curve	c2 = loc->C,
			c1 = t1 < tMin ? GetPreviousCurve(c2) : c2,
			c4 = loc->Inters->C,
			c3 = t2 < tMin ? GetPreviousCurve(c4) : c4;

	if (t1 > tMax)
		c2 = GetNextCurve(c2);
	if (t2 > tMax)
		c4 = GetNextCurve(c4);
	
	std::vector<double> offsets;

	if (!t1Inside)
	{
		AddOffsets(offsets, c1, true);
		AddOffsets(offsets, c2, false);
	}
	if (!t2Inside)
	{
		AddOffsets(offsets, c3, true);
		AddOffsets(offsets, c4, false);
	}

	PointD	pt = loc->C.GetPoint(loc->Time);
	double	offset = DBL_MAX;
	for (const auto& o : offsets)
		if (o < offset)
			offset = o;

	PointD	v2 = c2.GetTangent(t1, offset, t1Inside, pt),
			v1 = t1Inside ? PointD(-v2.X, -v2.Y)
						  : c1.GetTangent(t1, -offset, false, pt),
			v4 = c4.GetTangent(t2, offset, t2Inside, pt),
			v3 = t2Inside ? PointD(-v4.X, -v4.Y)
						  : c3.GetTangent(t2, -offset, false, pt);
	double	a1 = v1.Equals(PointD()) ? 0.0 : atan2(v1.Y, v1.X) * 180 / M_PI,
			a2 = v2.Equals(PointD()) ? 0.0 : atan2(v2.Y, v2.X) * 180 / M_PI,
			a3 = v3.Equals(PointD()) ? 0.0 : atan2(v3.Y, v3.X) * 180 / M_PI,
			a4 = v4.Equals(PointD()) ? 0.0 : atan2(v4.Y, v4.X) * 180 / M_PI;
	bool	inRange34 = isInRange(a1, a3, a4) ^ isInRange(a2, a3, a4),
			inRange43 = isInRange(a1, a4, a3) ^ isInRange(a2, a4, a3),
			inRange1 = inRange34 && inRange43,
			inRange12 = isInRange(a3, a1, a2) ^ isInRange(a4, a1, a2),
			inRange21 = isInRange(a3, a2, a1) ^ isInRange(a4, a2, a1),
			inRange2 = inRange21 && inRange12;

	return t1Inside ? inRange1 : inRange2;
}

bool CBooleanOperations::FilterIntersections(std::shared_ptr<Location> loc)
{
	return loc->Overlap || IsCrossing(loc);
}

bool CBooleanOperations::IntersectsBounds()
{
	RectF_T<double> rect1, rect2;
	Path1->GetBounds(rect1.X, rect1.Y, rect1.Width, rect1.Height);
	Path2->GetBounds(rect2.X, rect2.Y, rect2.Width, rect2.Height);

	return	(rect2.X + rect2.Width > rect1.X - EPSILON) &&
			(rect2.Y + rect2.Height > rect1.Y - EPSILON) &&
			(rect2.X < rect1.X + rect1.Width + EPSILON) &&
			(rect2.Y < rect1.Y + rect2.Height + EPSILON);
}

void CBooleanOperations::GetIntersection()
{
	if (!IntersectsBounds())
		return;

	std::vector<std::vector<int>> boundsCollisions =
		FindCurveBoundsCollisions(Curves1, Curves2, GEOMETRIC_EPSILON);
	for (int index1 = 0; index1 < Curves1.size(); index1++)
	{
		Curve curve1 = Curves1[index1];
		std::vector<int> collisions1 = boundsCollisions[index1];
		if (!collisions1.empty())
		{
			for (int j = 0; j < collisions1.size(); j++)
			{
				int index2 = collisions1[j];
				Curve curve2 = Curves2[index2];
				GetCurveIntersection(curve1, curve2);
			}
		}
	}
}

void CBooleanOperations::GetCurveIntersection(const Curve& curve1, const Curve& curve2)
{
	std::vector<double>	x1 = curve1.GetXValues(),
						y1 = curve1.GetYValues(),
						x2 = curve2.GetXValues(),
						y2 = curve2.GetYValues();

	double	minX1 = min(x1[0], x1[1], x1[2], x1[3]),
			maxX1 = max(x1[0], x1[1], x1[2], x1[3]),
			minY1 = min(y1[0], y1[1], y1[2], y1[3]),
			maxY1 = max(y1[0], y1[1], y1[2], y1[3]),
			minX2 = min(x2[0], x2[1], x2[2], x2[3]),
			maxX2 = max(x2[0], x2[1], x2[2], x2[3]),
			minY2 = min(y2[0], y2[1], y2[2], y2[3]),
			maxY2 = max(y2[0], y2[1], y2[2], y2[3]);

	if (maxX1 + EPSILON > minX2 && minX1 - EPSILON < maxX2 &&
		maxY1 + EPSILON > minY2 && minY1 - EPSILON < maxY2)
	{
		std::vector<std::pair<int, int>> overlaps =
			curve1.GetOverlaps(curve2);
		if (!overlaps.empty())
		{
			for (int i = 0; i < 2; i++)
			{
				std::pair<int, int> overlap = overlaps[i];
				AddLocation(curve1, curve2, overlap.first,
							overlap.second, true);
			}
		}
		else
		{
			bool	straight1 = curve1.IsStraight(),
					straight2 = curve2.IsStraight(),
					straight = straight1 && straight2,
					flip = straight1 && !straight2;
			int	before = Locations.size();
			Curve	c1 = flip ? curve2 : curve1,
					c2 = flip ? curve1 : curve2;
			if (straight)
				AddLineIntersection(c1, c2, flip);
			else if (straight1 || straight2)
				AddCurveLineIntersection(c1, c2, flip);
			else
				AddCurveIntersection(c1, c2, c1, c2, flip);
			if (!straight || Locations.size() == before)
			{
				for (int i = 0; i < 4; i++)
				{
					int t1 = i >> 1,
						t2 = i & 1;
					PointD	p1 = t1 == 0 ? curve1.Segment1.P
										 : curve1.Segment2.P,
							p2 = t2 == 0 ? curve2.Segment1.P
										 : curve2.Segment2.P;
					if (getDistance(p1, p2) <= EPSILON)
						AddLocation(curve1, curve2, t1, t2, false);
				}
			}
		}
	}
}

void CBooleanOperations::LinkIntersection(std::shared_ptr<Location> from,
										 std::shared_ptr<Location> to)
{
	std::shared_ptr<Location> prev = from;
	while (prev)
	{
		if (prev == to)
			return;
		prev = prev->Prev;
	}

	while (from->Next != nullptr && from->Next != to)
		from = from->Next;

	if (from->Next == nullptr)
	{
		while (to->Prev)
			to = to->Prev;
		from->Next = to;
		to->Prev = from;
	}
}

void CBooleanOperations::AddLineIntersection(const Curve& curve1, const Curve& curve2, bool flip)
{
	std::vector<double>	x1 = curve1.GetXValues(),
						y1 = curve1.GetYValues(),
						x2 = curve2.GetXValues(),
						y2 = curve2.GetYValues();
	PointD pt = intersect(x1[0], y1[0], x1[3], y1[3], x2[0], y2[0], x2[3], y2[3]);
	if (!pt.Equals(PointD()))
	{
		AddLocation(curve1, curve2, curve1.GetTimeOf(pt), curve2.GetTimeOf(pt));
	}
}

void CBooleanOperations::AddCurveLineIntersection(const Curve& curve1, const Curve& curve2, bool flip)
{
	std::vector<double>	x2 = curve2.GetXValues(),
						y2 = curve2.GetYValues(),
						roots = curve1.GetCurveLineIntersection(x2[0], y2[0],
																x2[3] - x2[0],
																y2[3] - y2[0]);
	for (const auto& r : roots)
	{
		double	t1 = r;
		PointD	p1 = curve1.GetPoint(t1);
		double	t2 = curve2.GetTimeOf(p1);
		if (flip) std::swap(t1, t2);
		if (t2 != -1)
			AddLocation(flip ? curve2 :curve1, flip ? curve1 : curve2, t1, t2);
	}
}

int CBooleanOperations::AddCurveIntersection(Curve curve1, Curve curve2, const Curve& startCurve1, const Curve& startCurve2, bool flip,
											int recursion, int calls, double tMin,
											double tMax, double uMin, double uMax)
{
	if (++calls >= 4096 || ++recursion >= 40)
		return calls;
	
	std::vector<double>	x1 = curve1.GetXValues(),
						y1 = curve1.GetYValues(),
						x2 = curve2.GetXValues(),
						y2 = curve2.GetYValues();
	double	d1 = getSignedDistance(x2[0], y2[0], x2[3], y2[3], x2[1], y2[1]),
			d2 = getSignedDistance(x2[0], y2[0], x2[3], y2[3], x2[2], y2[2]),
			factor = (d1 * d2) > 0 ? 3.0 / 4.0 : 4.0 / 9.0,
			dMin = factor * min(0, d1, d2),
			dMax = factor * max(0, d1, d2),
			dp0 = getSignedDistance(x2[0], y2[0], x2[3], y2[3], x1[0], y1[0]),
			dp1 = getSignedDistance(x2[0], y2[0], x2[3], y2[3], x1[1], y1[1]),
			dp2 = getSignedDistance(x2[0], y2[0], x2[3], y2[3], x1[2], y1[2]),
			dp3 = getSignedDistance(x2[0], y2[0], x2[3], y2[3], x1[3], y1[3]);
	std::vector<std::vector<PointD>> hull = getConvexHull(dp0, dp1, dp2, dp3);
	std::vector<PointD>	top = hull[0],
						bottom = hull[1];
	double tMinClip = clipConvexHull(top, bottom, dMin, dMax);
	std::reverse(top.begin(), top.end());
	std::reverse(bottom.begin(), bottom.end());
	double tMaxClip = clipConvexHull(top, bottom, dMin, dMax);
	if ((d1 == 0 && d2 == 0 && dp0 == 0 && dp1 == 0 && dp2 == 0 && dp3 == 0)
		|| tMinClip == DBL_MIN || tMaxClip == DBL_MIN)
		return calls;
	
	double	tMinNew = tMin + (tMax - tMin) * tMinClip,
			tMaxNew = tMin + (tMax - tMin) * tMaxClip;
	
	if (std::max(uMax - uMin, tMaxNew - tMinNew) < LINE_EPSILON)
	{
		double	t = (tMinNew + tMaxNew) / 2,
				u = (uMin + uMax) / 2;
		Curve	c1 = flip ? startCurve2 : startCurve1,
				c2 = flip ? startCurve1 : startCurve2;
		double	t1 = flip ? u : t,
				t2 = flip ? t : u;
		AddLocation(c1, c2, t1, t2);
	}
	else
	{
		curve1 = curve1.GetPart(tMinClip, tMaxClip);
		double uDiff = uMax - uMin;

		if (tMaxClip - tMinClip > 0.8)
		{
			if (tMaxNew - tMinNew > uDiff)
			{
				std::vector<Curve> parts = curve1.Subdivide(0.5);
				double t = (tMinNew + tMaxNew) / 2;
				parts[0].Segment2.HI.X -= parts[0].Segment2.P.X;
				parts[0].Segment2.HI.Y -= parts[0].Segment2.P.Y;
				parts[0].Segment2.HO.X -= parts[0].Segment2.P.X;
				parts[0].Segment2.HO.Y -= parts[0].Segment2.P.Y;
				parts[1].Segment2.HI.X -= parts[1].Segment2.P.X;
				parts[1].Segment2.HI.Y -= parts[1].Segment2.P.Y;
				parts[1].Segment2.HO.X -= parts[1].Segment2.P.X;
				parts[1].Segment2.HO.Y -= parts[1].Segment2.P.Y;
				calls = AddCurveIntersection(curve2, parts[0], startCurve2, startCurve1,
											!flip, recursion, calls, uMin, uMax, tMinNew, t);
				calls = AddCurveIntersection(curve2, parts[1], startCurve2, startCurve1,
											!flip, recursion, calls, uMin, uMax, t, tMaxNew);
			}
			else
			{
				std::vector<Curve> parts = curve2.Subdivide(0.5);
				double u = (uMin + uMax) / 2;
				parts[0].Segment2.HI.X -= parts[0].Segment2.P.X;
				parts[0].Segment2.HI.Y -= parts[0].Segment2.P.Y;
				parts[0].Segment2.HO.X -= parts[0].Segment2.P.X;
				parts[0].Segment2.HO.Y -= parts[0].Segment2.P.Y;
				parts[1].Segment2.HI.X -= parts[1].Segment2.P.X;
				parts[1].Segment2.HI.Y -= parts[1].Segment2.P.Y;
				parts[1].Segment2.HO.X -= parts[1].Segment2.P.X;
				parts[1].Segment2.HO.Y -= parts[1].Segment2.P.Y;
				calls = AddCurveIntersection(parts[0], curve1, startCurve2, startCurve1,
											!flip, recursion, calls, uMin, u, tMinNew, tMaxNew);
				calls = AddCurveIntersection(parts[1], curve1, startCurve2, startCurve1,
											!flip, recursion, calls, u, uMax, tMinNew, tMaxNew);
			}
		}
		else
		{
			if (uDiff == 0 || uDiff >= LINE_EPSILON)
				calls = AddCurveIntersection(curve2, curve1, startCurve2, startCurve1,
											!flip, recursion, calls, uMin, uMax, tMinNew, tMaxNew);
			else
				calls = AddCurveIntersection(curve1, curve2, startCurve1, startCurve2,
											flip, recursion, calls, tMinNew, tMaxNew, uMin, uMax);
		}
	}
	return calls;
}

void CBooleanOperations::DivideLocations()
{
	std::vector<std::shared_ptr<Location>> result;
	std::vector<Curve> clearCurves;
	bool	ClearHandles = false;
	Curve	prevCurve;
	double	tMin = CURVETIME_EPSILON,
			tMax = 1 - tMin,
			prevTime = -1.0;

	for (int i = Locations.size() - 1; i >= 0; i--)
	{
		std::shared_ptr<Location> loc = Locations[i];
		double	origTime = loc->Time,
				time = loc->Time;
		Segment segment, handles;
		Curve	curve = loc->C;
		Curve	newCurve;
		bool updateHandles = false;

		// if (loc->C != prevCurve)
		// {
		// 	ClearHandles = !curve.HasHandle();
		// 	prevCurve = curve;
		// }
		// else if (prevTime >= tMin)
		// {
		// 	time /= prevTime;
		// }

		// curve = loc->C.Segment1.Id == 1 ? Curves1[loc->C.Segment1.Index]
		// 								: Curves2[loc->C.Segment1.Index];
		// prevTime = origTime;
		if (time < tMin)
		{
			segment = curve.Segment1;
		}
		else if (time > tMax)
		{
			segment = curve.Segment2;
		}
		else
		{
			newCurve = curve.Segment1.Id == 1 ?
					   curve.DivideAtTime(time, Segments1, Curves1) :
					   curve.DivideAtTime(time, Segments2, Curves2);
			if (ClearHandles)
			{
				clearCurves.push_back(curve);
				clearCurves.push_back(newCurve);
			}			
			segment = newCurve.Segment1;
			updateHandles = true;
		}
		std::shared_ptr<Location>	inter = segment.Inters,
									dest = loc->Inters;
		if ((bool)inter)
		{
			LinkIntersection(inter, dest);
			std::shared_ptr<Location> other = inter;
			while ((bool)other)
			{
				LinkIntersection(other->Inters, inter);
				other = other->Next;
			}
		}
		else
			segment.Inters = dest;

		InsertSegment(segment, newCurve.Segment2, updateHandles);
		loc->S = segment;
	}
	ClearCurveHandles(clearCurves);
}

void CBooleanOperations::InsertLocation(std::shared_ptr<Location> loc, bool overlap)
{
	if (Locations.empty())
	{
		Locations.push_back(loc);
		return;
	}

	int	length = Locations.size(),
			l = 0,
			r = length - 1;
	while (l <= r)
	{
		int mid = (l + r) >> 1;
		std::shared_ptr<Location> loc1 = Locations[mid];

		if (getDistance(loc->C.GetPoint(loc->Time), loc1->C.GetPoint(loc1->Time)) <= GEOMETRIC_EPSILON
			&& loc->C.Segment1.Id == loc1->C.Segment1.Id)
			return;

		if (overlap)
		{
			if (getDistance(loc->C.GetPoint(loc->Time), loc->Inters->C.GetPoint(loc->Inters->Time)) > GEOMETRIC_EPSILON)
				return;

			for (int i = mid - 1; i >= -1; i--)
			{
				std::shared_ptr<Location> loc2 = Locations[((i % length) + length) % length];
				if (getDistance(loc->C.GetPoint(loc->Time), loc2->C.GetPoint(loc2->Time)) > GEOMETRIC_EPSILON)
					break;
				if (isZero(loc->C.GetPoint(loc->Time).X - loc2->C.GetPoint(loc2->Time).X) &&
					isZero(loc->C.GetPoint(loc->Time).Y - loc2->C.GetPoint(loc2->Time).Y) &&
					loc->C.Segment1.Id == loc2->C.Segment1.Id)
					return;
			}
			for (int i = mid + 1; i <= length; i++)
			{
				std::shared_ptr<Location> loc2 = Locations[((i % length) + length) % length];
				if (getDistance(loc->C.GetPoint(loc->Time), loc2->C.GetPoint(loc2->Time)) > GEOMETRIC_EPSILON)
					break;
				if (isZero(loc->C.GetPoint(loc->Time).X - loc2->C.GetPoint(loc2->Time).X) &&
					isZero(loc->C.GetPoint(loc->Time).Y - loc2->C.GetPoint(loc2->Time).Y) &&
					loc->C.Segment1.Id == loc2->C.Segment1.Id)
					return;
			}
		}

		double diffId = loc->C.Segment1.Id - loc1->C.Segment1.Id,
			   diffT = (loc->C.Segment1.Index + loc->Time) - (loc1->C.Segment1.Index + loc1->Time);
		bool  self = loc->C.Segment1.Id == loc1->C.Segment1.Id;
		double diff = self ? (diffT) : (diffId);
		if (diff < 0)
			r = mid - 1;
		else
			l = mid + 1;
	}
	Locations.insert(Locations.begin() + l, loc);
}

void CBooleanOperations::AddLocation(Curve curve1, Curve curve2,
									double t1, double t2, bool overlap)
{
	bool excludeStart = !overlap &&	GetPreviousCurve(curve1) == curve2,
		 excludeEnd = !overlap && curve1 != curve2 && GetNextCurve(curve1) == curve2;
	double	tMin = CURVETIME_EPSILON,
			tMax = 1 - tMin;

	if (t1 >= (excludeStart ? tMin : 0) &&
		t1 <= (excludeEnd ? tMax : 1))
	{
		if (t2 >= (excludeEnd ? tMin : 0) &&
			t2 <= (excludeStart ? tMax : 1))
		{
			std::shared_ptr<Location>	loc1(new Location(curve1, t1, overlap)),
										loc2(new Location(curve2, t2, overlap));
			loc1->Inters = loc2;
			loc2->Inters = loc1;
			if (FilterIntersections(loc1))
				InsertLocation(loc1, overlap);
		}
	}
}

void CBooleanOperations::ClearCurveHandles(std::vector<Curve>& curves)
{
	for (auto& c : curves)
		c.ClearHandles();
}

void CBooleanOperations::AddOffsets(std::vector<double>& offsets,
								   const Curve& curve, bool end)
{
	std::vector<double> roots = curve.GetPeeks();
	int	count = roots.size();
	bool	first = end && count != 0,
			second = !end && count != 0;
	double	offset = curve.GetLength(first ? roots[count - 1] : 0,
									second ? roots[0] : 1);
	offsets.push_back(count != 0 ? offset : offset / 32);
}

void CBooleanOperations::PropagateWinding(Segment segment,
										std::vector<std::vector<Curve>> curveCollisonsMap)
{
	std::vector<std::tuple<Segment, Curve, double>> chain;
	Segment start = segment;
	double totalLength = 0;
	Winding winding;
	winding.Quality = -1.0;

	do
	{
		Curve curve = GetCurve(segment);
		if (curve != Curve())
		{
			double length = curve.GetLength();
			totalLength += length;
			chain.push_back({segment, curve, length});
		}
		segment = GetNextSegment(segment);
	} while (segment != Segment() && !(bool)segment.Inters && segment != start);

	double	offsets[3] = {0.5, 0.25, 0.75},
			tMin = 1e-3,
			tMax = 1 - tMin;

	for (int i = 0; i < 3 && winding.Quality < 0.5; i++)
	{
		double length = totalLength * offsets[i];
		for (int j = 0; j < chain.size(); j++)
		{
			std::tuple<Segment, Curve, double> entry = chain[j];
			double curveLength = std::get<2>(entry);
			Winding wind, map;

			if (length <= curveLength)
			{
				Curve	curve = std::get<1>(entry);
				int	id = curve.Segment1.Id;
				double	t = clamp(curve.GetTimeAt(length), tMin, tMax);
				PointD	pt = curve.GetPoint(t);
				bool	dir = abs(curve.GetTangent(t).Y) < sqrt(1/2);
				map.SetDirection(dir);
				map.SetPoint(pt);
				map.Curves1 = Curves1;
				map.Curves2 = Curves2;
				
				if (Op == Subtraction)
				{
					map.SetCurves(Curves1, Curves2);
					double windPath = (std::max(map.PathWindingLeft,
											   map.PathWindingRight));

					if ((id == 1 && windPath != 0.0) ||
						(id == 2 && windPath == 0.0))
					{
						if (map.Quality < 1.0)
							continue;
						else
						{
							wind.W = 0.0;
							wind.Quality = 1.0;
						}
					}

					map.Reset();
				}

				int index = id == 1 ? curve.Segment1.Index
									   : Curves1.size() + curve.Segment1.Index;
				std::vector<Curve> newCurvesHor = curveCollisonsMap[2 * index],
								   newCurvesVer = curveCollisonsMap[2 * index + 1];
				map.SetCurves(newCurvesHor, newCurvesVer);
				map.GetWinding();
				
				wind.W = std::max(map.WindingLeft, map.WindingRight);
				wind.Quality = map.Quality;
				wind.WindingLeft = map.WindingLeft;
				wind.WindingRight = map.WindingRight;
				wind.OnPath = map.OnAnyPath ? true : false;

				if (wind.Quality > winding.Quality)
					winding = wind;
				break;
			}
			length -= curveLength;
		}
	}

	for (int j = chain.size() - 1; j >= 0; j--)
	{
		if (std::get<0>(chain[j]).Id == 1)
			Segments[std::get<0>(chain[j]).Index].Wind =
				std::make_shared<Winding>(winding);
		else
			Segments[std::get<0>(chain[j]).Index + Segments1.size()].Wind =
				std::make_shared<Winding>(winding);
	}
}

void CBooleanOperations::Collect(const Segment& seg, std::vector<Segment>& crossing,
								std::vector<Segment>& starts, std::shared_ptr<Location> inter,
								std::shared_ptr<Location> end, bool collectStarts) const
{
	while ((bool)inter && inter != end)
	{
		Segment other = inter->S.Id == 1 ? Segments[inter->S.Index]
										 : Segments[Segments1.size() + inter->S.Index],
				next;
		int last = other.Id == 1 ? Segments1.size() - 1
									: Segments2.size() - 1;
		if (other.Index == last)
			next = other.Id == 1 ? Segments[0] : Segments[Segments1.size()];
		else
			next = other.Id == 1 ? Segments[other.Index + 1]
								 : Segments[Segments1.size() + other.Index + 1];

		std::shared_ptr<Location> nextInter = next.Inters;
		Segment nextInterS;

		if ((bool)nextInter)
			nextInterS = nextInter->S.Id == 1 ? Segments[nextInter->S.Index]
											  : Segments[Segments1.size() + nextInter->S.Index];

		if (other != seg && ((other.Index == 0 && other.Id == 0) ||
			(next.Index == 0 && next.Id == 0) ||
			(other.IsValid(Op) && (next.IsValid(Op) ||
			(bool)nextInter && nextInterS.IsValid(Op)))))
			crossing.push_back(other);

		if (collectStarts)
			starts.push_back(other);

		inter = inter->Next;
	}
}

CGraphicsPath* BooleanOperation(CGraphicsPath *path1, CGraphicsPath *path2, BooleanOpType op)
{
	CBooleanOperations operation(path1, path2, op);
	return operation.GetResult();
}
}
