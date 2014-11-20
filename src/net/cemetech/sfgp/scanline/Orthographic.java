package net.cemetech.sfgp.scanline;

public class Orthographic extends Projection {

	Point min, max;
	
	public Orthographic(Point min, Point max) {
		setViewPort(min, max);
	}
	
	public void setViewPort(Point min, Point max) {
		this.min = min;
		this.max = max;
	}

	@Override
	public Point projectPoint(Point p) {
		int x = p.getComponent(Point.CoordName.X);
		int y = p.getComponent(Point.CoordName.Y);
		int z = p.getComponent(Point.CoordName.Z);
		
		int l = min.getComponent(Point.CoordName.X);
		int r = max.getComponent(Point.CoordName.X);
		int b = min.getComponent(Point.CoordName.Y);
		int t = max.getComponent(Point.CoordName.Y);
		int n = min.getComponent(Point.CoordName.Z);
		int f = max.getComponent(Point.CoordName.Z);
		
		return new Point((2*x - (l + r))/(r-l),(2*y - (t + b))/(t-b),(-2*z + (f + n))/(f-n));
	}

}
