package net.cemetech.sfgp.scanline;

public class Compose extends Projection {
	
	Projection left;
	Projection right;
	
	public Compose(Projection l, Projection r){
		left = l; right = r;
	}

	@Override
	public Point projectPoint(Point p) {
		return left.projectPoint(right.projectPoint(p));
	}

}
