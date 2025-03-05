export interface Menu {
  id: string;
  name: string;
  price: number;
  active: boolean;
  RawMaterial_MenuItems: RawMaterialMenuItem[];
}

interface RawMaterialMenuItem {
  id: string;
  quantity: number;
  rawMaterialId: string;
  menuItemId: string;
  RawMaterial: RawMaterial;
}

interface RawMaterial {
  id: string;
  name: string;
  quantity: number;
  unit: string;
}
