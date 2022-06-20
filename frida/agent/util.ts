export function log(message: string): void {
    console.log(message)
}

export function in_range(val: number, min: number, len: number) {
    return (val >= min && val <= (min + len))
}

export function align(h: number, align: number): number {
    var a = align -1;
    return ((h + a) & ~(a))
}  